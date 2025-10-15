#include "SvgDocument.h"
#include "SvgElementFactory.h"
#include "SvgElement.h"
#include "SvgTransform.h"
#include "SvgRect.h"
#include "SvgStyle.h"
#include "SvgGroup.h"
#include <QDomDocument>
#include <QFile>
#include <QXmlStreamReader>
#include <QRegularExpression>

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

bool SvgDocument::load(const QString& filePath) {
    // 清空原有数据（递归删除所有元素）
    qDeleteAll(mElements);
    mElements.clear();
    mIsValid = false;
    mViewBox = QRectF();

    // 加载并解析XML文件
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

    // 解析根元素（必须是svg标签）
    QDomElement rootElem = domDoc.documentElement();
    if (rootElem.tagName().toLower() != "svg") {
        qDebug() << "根元素不是svg标签";
        return false;
    }

    // 1. 解析viewBox属性（优先使用SVG中定义的）
    QString viewBoxStr = rootElem.attribute("viewBox");
    if (!viewBoxStr.isEmpty()) {
        QStringList parts = viewBoxStr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() == 4) {
            bool xOk, yOk, wOk, hOk;
            qreal x = parts[0].toDouble(&xOk);
            qreal y = parts[1].toDouble(&yOk);
            qreal w = parts[2].toDouble(&wOk);
            qreal h = parts[3].toDouble(&hOk);
            if (xOk && yOk && wOk && hOk && w > 0 && h > 0) {
                mViewBox = QRectF(x, y, w, h);
                qDebug() << "从SVG解析到viewBox：" << mViewBox;
            } else {
                qDebug() << "viewBox数值无效，将计算默认值";
            }
        } else {
            qDebug() << "viewBox格式错误（需4个数值），将计算默认值";
        }
    } else {
        qDebug() << "SVG无viewBox属性，将计算默认值";
    }

    // 2. 通过工厂类创建根元素及其所有子元素（递归解析）
    SvgElement* root = SvgElementFactory::createElement(rootElem, this); // 传入document指针
    if (root) {
        mElements.append(root); // 根元素存入文档（可能是svg或g等容器元素）
    } else {
        qDebug() << "根元素创建失败";
        return false;
    }

    // 3. 若viewBox无效，计算默认值（包含所有元素的最小边界框）
    if (mViewBox.width() <= 0 || mViewBox.height() <= 0) {
        calculateDefaultViewBox();
    }

    // 4. 验证文档有效性（有元素且viewBox有效）
    mIsValid = !mElements.isEmpty() && mViewBox.width() > 0 && mViewBox.height() > 0;
    qDebug() << "SVG加载完成，是否有效：" << mIsValid
             << "，元素数量：" << totalElementCount() // 新增：统计所有元素（含子元素）
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

// 新增：辅助函数，统计所有元素（含嵌套子元素）的数量
int SvgDocument::totalElementCount() const {
    int count = 0;
    for (SvgElement* elem : mElements) {
        count += countElementRecursive(elem);
    }
    return count;
}

// 递归统计单个元素及其子元素数量
int SvgDocument::countElementRecursive(SvgElement* elem) const {
    if (!elem) return 0;
    int count = 1; // 自身
    // 若为容器元素（如g），递归统计子元素
    if (elem->type() == SvgElement::TypeGroup) {
        auto* group = static_cast<SvgGroup*>(elem);
        for (SvgElement* child : group->children()) {
            count += countElementRecursive(child);
        }
    }
    return count;
}

// 辅助：递归扩展边界框以包含元素及其子元素
void SvgDocument::expandBboxWithElement(SvgElement* elem, QRectF& bbox) const {
    if (!elem) return;
    // 合并当前元素的边界框
    bbox = bbox.united(elem->boundingBox());
    // 若为容器元素，递归处理子元素
    if (elem->type() == SvgElement::TypeGroup) {
        auto* group = static_cast<SvgGroup*>(elem);
        for (SvgElement* child : group->children()) {
            expandBboxWithElement(child, bbox);
        }
    }
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
        QRectF elemBbox = elem->boundingBox();  // 调用每个元素自身的 boundingBox()
        if (totalBounds.isEmpty()) {
            totalBounds = elemBbox;
        } else {
            totalBounds = totalBounds.united(elemBbox);
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

bool SvgDocument::parseSvgElement(const QDomElement& domElement)
{
    // 调用工厂创建元素
    SvgElement* element = SvgElementFactory::createElement(domElement, this);
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
    return SvgElementFactory::createElement(domElement, this);
}
