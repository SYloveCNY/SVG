#include "SvgElementFactory.h"
#include "SvgRect.h"
#include "SvgCircle.h"
#include "SvgText.h"
#include "SvgGroup.h"
#include "SvgTransform.h"
#include "SvgStyle.h"
#include <QDomElement>

SvgElement* SvgElementFactory::createElement(const QDomElement& domElement)
{
    const QString tagName = domElement.tagName().toLower(); // 不区分大小写

    // 根据标签名创建对应元素
    if (tagName == "rect") {
        return createRectElement(domElement);
    } else if (tagName == "circle") {
        return createCircleElement(domElement);
    } else if (tagName == "text") {
        return createTextElement(domElement);
    } else if (tagName == "g") {
        return createGroupElement(domElement);
    }

    // 未支持的元素类型
    return nullptr;
}

void SvgElementFactory::parseCommonAttributes(SvgElement* element, const QDomElement& domElement)
{
    if (!element) return;

    // 1. 解析ID
    if (domElement.hasAttribute("id")) {
        element->setId(domElement.attribute("id"));
    }

    // 2. 解析Transform
    if (domElement.hasAttribute("transform")) {
        SvgTransform transform;
        transform.parseTransform(domElement.attribute("transform"));
        element->setTransform(transform);
    }

    // 3. 解析Style（inline style或style属性）
    SvgStyle style;
    if (domElement.hasAttribute("style")) {
        style.parseStyleString(domElement.attribute("style"));
    }
    // 补充：解析单独的style属性（如fill、stroke）
    if (domElement.hasAttribute("fill")) {
        style.parseAttribute("fill", domElement.attribute("fill"));
    }
    if (domElement.hasAttribute("stroke")) {
        style.parseAttribute("stroke", domElement.attribute("stroke"));
    }
    if (domElement.hasAttribute("stroke-width")) {
        style.parseAttribute("stroke-width", domElement.attribute("stroke-width"));
    }
    element->setStyle(style);
}

SvgElement* SvgElementFactory::createRectElement(const QDomElement& domElement)
{
    auto* rect = new SvgRect();
    parseCommonAttributes(rect, domElement);

    // 解析矩形特有属性
    if (domElement.hasAttribute("x")) rect->setX(domElement.attribute("x").toDouble());
    if (domElement.hasAttribute("y")) rect->setY(domElement.attribute("y").toDouble());
    if (domElement.hasAttribute("width")) rect->setWidth(domElement.attribute("width").toDouble());
    if (domElement.hasAttribute("height")) rect->setHeight(domElement.attribute("height").toDouble());

    // 更新边界框
    rect->setBoundingBox(rect->boundingBox());
    return rect;
}

SvgElement* SvgElementFactory::createCircleElement(const QDomElement& domElement)
{
    auto* circle = new SvgCircle();
    parseCommonAttributes(circle, domElement);

    // 解析圆形特有属性
    QPointF center;
    if (domElement.hasAttribute("cx")) center.setX(domElement.attribute("cx").toDouble());
    if (domElement.hasAttribute("cy")) center.setY(domElement.attribute("cy").toDouble());
    circle->setCenter(center);

    if (domElement.hasAttribute("r")) circle->setRadius(domElement.attribute("r").toDouble());

    // 更新边界框
    circle->setBoundingBox(circle->boundingBox());
    return circle;
}

SvgElement* SvgElementFactory::createTextElement(const QDomElement& domElement)
{
    auto* text = new SvgText();
    parseCommonAttributes(text, domElement);

    // 解析文本特有属性
    QPointF pos;
    if (domElement.hasAttribute("x")) pos.setX(domElement.attribute("x").toDouble());
    if (domElement.hasAttribute("y")) pos.setY(domElement.attribute("y").toDouble());
    text->setPosition(pos);

    // 解析文本内容
    text->setText(domElement.text().trimmed());

    // 更新边界框（简化：实际需计算文本宽度）
    text->setBoundingBox(text->boundingBox());
    return text;
}

SvgElement* SvgElementFactory::createGroupElement(const QDomElement& domElement)
{
    auto* group = new SvgGroup();
    parseCommonAttributes(group, domElement);

    // 递归解析组内子元素
    QDomNode childNode = domElement.firstChild();
    while (!childNode.isNull()) {
        if (childNode.isElement()) {
            QDomElement childElem = childNode.toElement();
            SvgElement* child = createElement(childElem); // 递归调用工厂
            if (child) {
                group->addChild(child);
            }
        }
        childNode = childNode.nextSibling();
    }

    // 更新组边界框（合并所有子元素边界框）
    group->setBoundingBox(group->boundingBox());
    return group;
}
