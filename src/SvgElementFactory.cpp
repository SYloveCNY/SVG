#include "SvgElementFactory.h"
#include "SvgRect.h"
#include "SvgCircle.h"
#include "SvgText.h"
#include "SvgGroup.h"
#include "SvgEllipse.h"   // 新增：椭圆元素
#include "SvgLine.h"      // 新增：直线元素
#include "SvgPolyline.h"  // 新增：折线元素
#include "SvgPolygon.h"   // 新增：多边形元素
#include "SvgPath.h"      // 新增：路径元素
#include "SvgTransform.h"
#include "SvgStyle.h"
#include <QDomElement>
#include <QDebug>
#include <QRegularExpression>

// 核心：扩展标签识别
SvgElement* SvgElementFactory::createElement(const QDomElement& domElement)
{
    const QString tagName = domElement.tagName().toLower();

    if (tagName == "rect") {
        return createRectElement(domElement);
    } else if (tagName == "circle") {
        return createCircleElement(domElement);
    } else if (tagName == "text") {
        return createTextElement(domElement);
    } else if (tagName == "g") {
        return createGroupElement(domElement);
    } else if (tagName == "ellipse") {  // 新增：椭圆
        return createEllipseElement(domElement);
    } else if (tagName == "line") {     // 新增：直线
        return createLineElement(domElement);
    } else if (tagName == "polyline") { // 新增：折线
        return createPolylineElement(domElement);
    } else if (tagName == "polygon") {  // 新增：多边形
        return createPolygonElement(domElement);
    } else if (tagName == "path") {     // 新增：路径
        return createPathElement(domElement);
    }

    qDebug() << "未支持的元素：" << tagName;
    return nullptr;
}

// 通用属性解析（无需修改，已支持所有元素的通用样式/变换）
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

    // 3. 解析样式（复用现有逻辑，支持inline style和单独属性）
    SvgStyle style;
    if (domElement.hasAttribute("style")) {
        style.parseStyleString(domElement.attribute("style"));
    }
    // 解析单独的样式属性（fill/stroke等）
    if (domElement.hasAttribute("fill")) {
        QString fillValue = domElement.attribute("fill");
        style.parseAttribute("fill", fillValue);
        qDebug() << "解析fill：" << fillValue;
    }
    if (domElement.hasAttribute("stroke")) {
        QString strokeValue = domElement.attribute("stroke");
        style.parseAttribute("stroke", strokeValue);
        qDebug() << "解析stroke：" << strokeValue;
    }
    if (domElement.hasAttribute("stroke-width")) {
        QString strokeWidthValue = domElement.attribute("stroke-width");
        style.parseAttribute("stroke-width", strokeWidthValue);
        qDebug() << "解析stroke-width：" << strokeWidthValue;
    }

    element->setStyle(style);
    qDebug() << "设置元素样式完成，填充：" << style.fill().name()
             << "描边：" << style.stroke().name();
}

// 椭圆元素创建与属性解析
SvgElement* SvgElementFactory::createEllipseElement(const QDomElement& domElement)
{
    auto* ellipse = new SvgEllipse();
    parseCommonAttributes(ellipse, domElement);

    // 解析椭圆特有属性：cx(中心x)、cy(中心y)、rx(x轴半径)、ry(y轴半径)
    if (domElement.hasAttribute("cx")) {
        ellipse->setCx(domElement.attribute("cx").toDouble());
    }
    if (domElement.hasAttribute("cy")) {
        ellipse->setCy(domElement.attribute("cy").toDouble());
    }
    if (domElement.hasAttribute("rx")) {
        ellipse->setRx(domElement.attribute("rx").toDouble());
    }
    if (domElement.hasAttribute("ry")) {
        ellipse->setRy(domElement.attribute("ry").toDouble());
    }

    // 设置边界框（椭圆的外接矩形）
    ellipse->setBoundingBox(QRectF(
        ellipse->cx() - ellipse->rx(),  // x
        ellipse->cy() - ellipse->ry(),  // y
        ellipse->rx() * 2,              // width
        ellipse->ry() * 2               // height
        ));
    qDebug() << "创建椭圆元素：中心(" << ellipse->cx() << "," << ellipse->cy()
             << "), 半径(" << ellipse->rx() << "," << ellipse->ry() << ")";
    return ellipse;
}

// 直线元素创建与属性解析
SvgElement* SvgElementFactory::createLineElement(const QDomElement& domElement)
{
    auto* line = new SvgLine();
    parseCommonAttributes(line, domElement);

    // 解析直线特有属性：x1,y1(起点)、x2,y2(终点)
    if (domElement.hasAttribute("x1")) {
        line->setX1(domElement.attribute("x1").toDouble());
    }
    if (domElement.hasAttribute("y1")) {
        line->setY1(domElement.attribute("y1").toDouble());
    }
    if (domElement.hasAttribute("x2")) {
        line->setX2(domElement.attribute("x2").toDouble());
    }
    if (domElement.hasAttribute("y2")) {
        line->setY2(domElement.attribute("y2").toDouble());
    }

    // 设置边界框（包含直线的最小矩形）
    qreal minX = qMin(line->x1(), line->x2());
    qreal minY = qMin(line->y1(), line->y2());
    qreal maxX = qMax(line->x1(), line->x2());
    qreal maxY = qMax(line->y1(), line->y2());
    line->setBoundingBox(QRectF(minX, minY, maxX - minX, maxY - minY));
    qDebug() << "创建直线元素：起点(" << line->x1() << "," << line->y1()
             << "), 终点(" << line->x2() << "," << line->y2() << ")";
    return line;
}

// 折线元素创建与属性解析
SvgElement* SvgElementFactory::createPolylineElement(const QDomElement& domElement)
{
    auto* polyline = new SvgPolyline();
    parseCommonAttributes(polyline, domElement);

    // 解析折线特有属性：points(坐标列表，如"0,0 100,50 200,0")
    if (domElement.hasAttribute("points")) {
        QList<QPointF> points = parsePoints(domElement.attribute("points"));
        polyline->setPoints(points);
    }

    // 设置边界框（包含所有点的最小矩形）
    polyline->setBoundingBox(calculatePointsBoundingBox(polyline->points()));
    qDebug() << "创建折线元素：" << polyline->points().size() << "个点";
    return polyline;
}

// 多边形元素创建与属性解析（与折线类似，但自动闭合）
SvgElement* SvgElementFactory::createPolygonElement(const QDomElement& domElement)
{
    auto* polygon = new SvgPolygon();
    parseCommonAttributes(polygon, domElement);

    // 解析多边形特有属性：points(坐标列表)
    if (domElement.hasAttribute("points")) {
        QList<QPointF> points = parsePoints(domElement.attribute("points"));
        polygon->setPoints(points);
    }

    // 设置边界框
    polygon->setBoundingBox(calculatePointsBoundingBox(polygon->points()));
    qDebug() << "创建多边形元素：" << polygon->points().size() << "个点";
    return polygon;
}

// 路径元素创建与属性解析（最复杂，需解析d属性）
SvgElement* SvgElementFactory::createPathElement(const QDomElement& domElement)
{
    auto* path = new SvgPath();
    parseCommonAttributes(path, domElement);

    // 解析路径特有属性：d(路径命令，如"M10,10 L100,10 Z")
    if (domElement.hasAttribute("d")) {
        QString d = domElement.attribute("d");
        QPainterPath painterPath = parsePathData(d);
        path->setPath(painterPath);
    }

    // 设置边界框（路径的外接矩形）
    path->setBoundingBox(path->path().boundingRect());
    qDebug() << "创建路径元素：" << path->path().elementCount() << "个路径段";
    return path;
}

// 辅助函数：解析points属性（将"x1,y1 x2,y2"转换为QPointF列表）
QList<QPointF> SvgElementFactory::parsePoints(const QString& pointsStr)
{
    QList<QPointF> points;
    if (pointsStr.isEmpty()) return points;

    // 按空格拆分每个点（支持多个空格分隔）
    QStringList pointList = pointsStr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    foreach (const QString& pointStr, pointList) {
        // 按逗号拆分x和y
        QStringList coords = pointStr.split(',');
        if (coords.size() == 2) {
            bool okX, okY;
            qreal x = coords[0].toDouble(&okX);
            qreal y = coords[1].toDouble(&okY);
            if (okX && okY) {
                points.append(QPointF(x, y));
            }
        }
    }
    return points;
}

// 辅助函数：计算点列表的边界框
QRectF SvgElementFactory::calculatePointsBoundingBox(const QList<QPointF>& points)
{
    if (points.isEmpty()) return QRectF();

    qreal minX = points[0].x(), maxX = points[0].x();
    qreal minY = points[0].y(), maxY = points[0].y();

    foreach (const QPointF& p, points) {
        minX = qMin(minX, p.x());
        maxX = qMax(maxX, p.x());
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }
    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

// 辅助函数：解析路径d属性（转换为QPainterPath）
QPainterPath SvgElementFactory::parsePathData(const QString& d) {
    QPainterPath path;
    if (d.isEmpty()) return path;

    // 路径命令正则（匹配M/L/C/Z等）
    QRegularExpression cmdRegex("[MLCZAQHVLmlczaqhvl]");
    // 数值正则（支持整数、小数、科学计数法）
    QRegularExpression numRegex("-?\\d+(\\.\\d+)?(e[-+]?\\d+)?");

    int pos = 0;
    QPointF currentPos;
    bool isRelative = false;

    while (pos < d.size()) {
        // 跳过空白字符
        while (pos < d.size() && d[pos].isSpace()) pos++;
        if (pos >= d.size()) break;

        // 匹配命令（替换indexIn为match）
        QRegularExpressionMatch cmdMatch = cmdRegex.match(d, pos);
        if (cmdMatch.hasMatch()) {  // 命令匹配成功
            QChar cmd = cmdMatch.captured(0).at(0);  // 用captured(0)获取匹配内容
            isRelative = cmd.isLower();
            char cmdUpper = cmd.toUpper().toLatin1();
            pos += cmdMatch.capturedLength();  // 用capturedLength()获取匹配长度

            // 提取命令参数（数值）
            QList<qreal> params;
            while (pos < d.size()) {
                // 跳过空白
                while (pos < d.size() && d[pos].isSpace()) pos++;
                if (pos >= d.size()) break;

                // 匹配数值（替换indexIn为match）
                QRegularExpressionMatch numMatch = numRegex.match(d, pos);
                if (!numMatch.hasMatch()) break;  // 无数值则退出

                params.append(numMatch.captured(0).toDouble());  // 用captured(0)获取数值
                pos += numMatch.capturedLength();  // 移动到下一个位置
            }

            // 处理命令（M/L/C/Z等，逻辑不变）
            switch (cmdUpper) {
            case 'M': // 移动到
                if (params.size() >= 2) {
                    qreal x = params[0], y = params[1];
                    if (isRelative) { x += currentPos.x(); y += currentPos.y(); }
                    path.moveTo(x, y);
                    currentPos = QPointF(x, y);
                }
                break;
                // ... 其他命令（L/C/Z）处理同上 ...
            }
        } else {
            pos++;  // 非命令字符，跳过
        }
    }
    return path;
}

// 以下为原有元素的创建函数（保持不变）
SvgElement* SvgElementFactory::createRectElement(const QDomElement& domElement)
{
    auto* rect = new SvgRect();
    parseCommonAttributes(rect, domElement);

    if (domElement.hasAttribute("x")) rect->setX(domElement.attribute("x").toDouble());
    if (domElement.hasAttribute("y")) rect->setY(domElement.attribute("y").toDouble());
    if (domElement.hasAttribute("width")) rect->setWidth(domElement.attribute("width").toDouble());
    if (domElement.hasAttribute("height")) rect->setHeight(domElement.attribute("height").toDouble());

    rect->setBoundingBox(QRectF(rect->x(), rect->y(), rect->width(), rect->height()));
    return rect;
}

SvgElement* SvgElementFactory::createCircleElement(const QDomElement& domElement)
{
    auto* circle = new SvgCircle();
    parseCommonAttributes(circle, domElement);

    QPointF center;
    if (domElement.hasAttribute("cx")) center.setX(domElement.attribute("cx").toDouble());
    if (domElement.hasAttribute("cy")) center.setY(domElement.attribute("cy").toDouble());
    circle->setCenter(center);

    if (domElement.hasAttribute("r")) circle->setRadius(domElement.attribute("r").toDouble());

    circle->setBoundingBox(QRectF(
        center.x() - circle->radius(),
        center.y() - circle->radius(),
        circle->radius() * 2,
        circle->radius() * 2
        ));
    return circle;
}

SvgElement* SvgElementFactory::createTextElement(const QDomElement& domElement)
{
    auto* text = new SvgText();
    parseCommonAttributes(text, domElement);

    QPointF pos;
    if (domElement.hasAttribute("x")) pos.setX(domElement.attribute("x").toDouble());
    if (domElement.hasAttribute("y")) pos.setY(domElement.attribute("y").toDouble());
    text->setPosition(pos);

    text->setText(domElement.text().trimmed());
    text->setBoundingBox(QRectF(pos.x(), pos.y() - 12, text->text().size() * 8, 16)); // 简化计算
    return text;
}

SvgElement* SvgElementFactory::createGroupElement(const QDomElement& domElement)
{
    auto* group = new SvgGroup();
    parseCommonAttributes(group, domElement);

    QDomNode childNode = domElement.firstChild();
    while (!childNode.isNull()) {
        if (childNode.isElement()) {
            SvgElement* child = createElement(childNode.toElement());
            if (child) group->addChild(child);
        }
        childNode = childNode.nextSibling();
    }

    // 计算组的边界框（合并所有子元素的边界框）
    QRectF groupBbox;
    foreach (const SvgElement* child, group->children()) {
        if (groupBbox.isEmpty()) {
            groupBbox = child->boundingBox();
        } else {
            groupBbox = groupBbox.united(child->boundingBox());
        }
    }
    group->setBoundingBox(groupBbox);
    return group;
}
