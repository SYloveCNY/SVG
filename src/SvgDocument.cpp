#include "SvgDocument.h"
#include "SvgElementFactory.h"
#include "SvgElement.h"
#include "SvgTransform.h"
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

bool SvgDocument::load(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    return loadFromData(data);
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

QList<SvgElement*> SvgDocument::elements() const
{
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
        return true;
    }
    return false;
}

SvgElement* SvgDocument::createElementFromDom(const QDomElement& domElement)
{
    // 委托给工厂类创建元素（核心修改）
    return SvgElementFactory::createElement(domElement);
}
