#include "SvgDocument.h"
#include "SvgElementFactory.h"
#include "SvgElement.h"
#include "SvgTransform.h"
#include "SvgRect.h"
#include "SvgStyle.h"
#include <QDomDocument>
#include <QFile>
#include <QXmlStreamReader>
#include <QRegularExpression>  // 使用QRegularExpression替代QRegExp

SvgDocument::SvgDocument()
{
}

SvgDocument::~SvgDocument()
{
    foreach (SvgElement* element, mElements) {
        delete element;
    }
    mElements.clear();
}

// bool SvgDocument::load(const QString& fileName)
// {
//     QFile file(fileName);
//     if (!file.open(QIODevice::ReadOnly)) {
//         return false;
//     }

//     QByteArray data = file.readAll();
//     file.close();

//     return loadFromData(data);
// }

bool SvgDocument::load(const QString& filePath) {
    // 清空原有数据
    for (auto elem : mElements) {
        delete elem;
    }
    mElements.clear();
    mIsValid = false; // 初始化为无效
    mViewBox = QRectF(); // 重置viewBox

    // 加载并解析XML
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件：" << filePath;
        return false;
    }

    QDomDocument domDoc;
    if (!domDoc.setContent(&file)) {
        file.close();
        qDebug() << "XML解析失败：" << filePath;
        return false;
    }
    file.close();

    // 解析根元素
    QDomElement root = domDoc.documentElement();
    if (root.tagName() != "svg") {
        qDebug() << "根元素不是svg标签";
        return false;
    }

    // 1. 解析viewBox属性
    QString viewBoxStr = root.attribute("viewBox");
    if (!viewBoxStr.isEmpty()) {
        QStringList parts = viewBoxStr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() == 4) {
            qreal x = parts[0].toDouble();
            qreal y = parts[1].toDouble();
            qreal w = parts[2].toDouble();
            qreal h = parts[3].toDouble();
            // 仅当宽高>0时才认为有效
            if (w > 0 && h > 0) {
                mViewBox = QRectF(x, y, w, h);
                qDebug() << "从SVG解析到viewBox：" << mViewBox;
            } else {
                qDebug() << "viewBox宽高无效，将计算默认值";
            }
        } else {
            qDebug() << "viewBox格式错误（需4个数值），将计算默认值";
        }
    } else {
        qDebug() << "SVG无viewBox属性，将计算默认值";
    }

    // 2. 解析所有子元素（rect等）
    parseSvgElement(root);

    // 3. 如果解析的viewBox无效，计算默认viewBox（包含所有元素的最小范围）
    if (mViewBox.width() <= 0 || mViewBox.height() <= 0) {
        calculateDefaultViewBox();
    }

    // 4. 最终验证文档有效性
    mIsValid = !mElements.isEmpty() && mViewBox.width() > 0 && mViewBox.height() > 0;
    qDebug() << "SVG加载完成，是否有效：" << mIsValid
             << "，元素数量：" << mElements.size()
             << "，最终viewBox：" << mViewBox;

    return mIsValid;
}

bool SvgDocument::loadFromData(const QByteArray& data)
{
    // 清空现有元素
    foreach (SvgElement* element, mElements) {
        delete element;
    }
    mElements.clear();

    QDomDocument domDoc;
    QString errorMsg;
    int errorLine, errorColumn;

    if (!domDoc.setContent(data, &errorMsg, &errorLine, &errorColumn)) {
        return false;
    }

    QDomElement root = domDoc.documentElement();
    if (root.tagName() != "svg") {
        return false;
    }

    // 解析 viewBox，使用QRegularExpression替代QRegExp
    if (root.hasAttribute("viewBox")) {
        QString viewBoxStr = root.attribute("viewBox");
        // 使用QRegularExpression分割空白字符
        QStringList parts = viewBoxStr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() == 4) {
            bool ok;
            qreal x = parts[0].toDouble(&ok); if (!ok) return false;
            qreal y = parts[1].toDouble(&ok); if (!ok) return false;
            qreal w = parts[2].toDouble(&ok); if (!ok) return false;
            qreal h = parts[3].toDouble(&ok); if (!ok) return false;
            mViewBox = QRectF(x, y, w, h);
        }
    }

    // 解析子元素
    QDomNode node = root.firstChild();
    while (!node.isNull()) {
        if (node.isElement()) {
            QDomElement elem = node.toElement();
            SvgElement* svgElement = createElementFromDom(elem);
            if (svgElement) {
                mElements.append(svgElement);
            }
        }
        node = node.nextSibling();
    }

    return true;
}

void SvgDocument::addElement(SvgElement* element)
{
    if (element) {
        mElements.append(element);
    }
}

void SvgDocument::removeElement(SvgElement* element)
{
    if (element) {
        mElements.removeAll(element);
        delete element;
    }
}

QList<SvgElement*> SvgDocument::elements() const {
    qDebug() << "SvgDocument::elements() 返回数量：" << mElements.size();
    return mElements;
}

QRectF SvgDocument::viewBox() const
{
    return mViewBox;
}

void SvgDocument::setViewBox(const QRectF& viewBox)
{
    mViewBox = viewBox;
}

// 计算默认viewBox：包含所有元素的最小矩形
void SvgDocument::calculateDefaultViewBox() {
    if (mElements.isEmpty()) {
        mViewBox = QRectF(0, 0, 200, 200);  // 无元素时默认200x200
        qDebug() << "计算默认viewBox（无元素）：" << mViewBox;
        return;
    }

    QRectF totalBounds;  // 所有元素的总边界

    foreach (const SvgElement* elem, mElements) {
        if (elem->type() == SvgElement::TypeShape) {
            const SvgRect* rect = dynamic_cast<const SvgRect*>(elem);
            if (rect) {
                QRectF elemRect(rect->x(), rect->y(), rect->width(), rect->height());
                if (totalBounds.isEmpty()) {
                    totalBounds = elemRect;
                } else {
                    totalBounds = totalBounds.united(elemRect);  // 合并边界
                }
            }
        }
    }

    // 确保viewBox有效（宽高>0）
    if (totalBounds.width() > 0 && totalBounds.height() > 0) {
        mViewBox = totalBounds;
    } else {
        mViewBox = QRectF(0, 0, 200, 200);
    }

    qDebug() << "计算默认viewBox（含元素）：" << mViewBox;
}

QString SvgDocument::title() const
{
    return mTitle;
}

void SvgDocument::setTitle(const QString& title)
{
    mTitle = title;
}

QString SvgDocument::description() const
{
    return mDescription;
}

void SvgDocument::setDescription(const QString& description)
{
    mDescription = description;
}

bool SvgDocument::parseSvgElement(const QDomElement& domElement)
{
    // 调用工厂创建元素
    SvgElement* element = SvgElementFactory::createElement(domElement);
    if (element) {
        mElements.append(element);
        qDebug() << "添加元素：" << domElement.tagName()
                 << "，当前元素总数：" << mElements.size();
    }
    else{
        qDebug() << "未支持的元素：" << domElement.tagName();
    }

    QDomNode childNode = domElement.firstChild();
    while (!childNode.isNull()) {
        if (childNode.isElement()) {
            QDomElement childElement = childNode.toElement();
            parseSvgElement(childElement);  // 必须执行：确保子元素被解析
        }
        childNode = childNode.nextSibling();
    }

    return true;
}

SvgElement* SvgDocument::createElementFromDom(const QDomElement& domElement)
{
    // 委托给工厂类创建元素（核心修改）
    return SvgElementFactory::createElement(domElement);
}
