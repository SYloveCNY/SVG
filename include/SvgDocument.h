#ifndef SVGDocument_H
#define SVGDocument_H

#include <QList>
#include <QRectF>
#include <QString>
#include "SvgElement.h"

class QDomDocument;
class QDomElement;

class SvgDocument
{
public:
    SvgDocument();
    ~SvgDocument();

    bool load(const QString& fileName);
    bool isValid() const { return mIsValid; }
    bool loadFromData(const QByteArray& data);

    void addElement(SvgElement* element);
    void removeElement(SvgElement* element);
    QList<SvgElement*> elements() const;

    QRectF viewBox() const;
    void setViewBox(const QRectF& viewBox);

    QString title() const;
    void setTitle(const QString& title);

    QString description() const;
    void setDescription(const QString& description);

    // 根元素访问
    SvgElement* rootElement() const { return m_rootElement; }

    // 1. 声明元素计数函数（与.cpp实现严格匹配）
    int totalElementCount() const;

    // 2. 声明递归计数辅助函数
    int countElementRecursive(SvgElement* elem) const;

    // 3. 声明边界框扩展函数
    void expandBboxWithElement(SvgElement* elem, QRectF& bbox) const;

    // 4. 声明默认viewBox计算函数
    void calculateDefaultViewBox();

private:
    bool parseSvgElement(const QDomElement& domElement);
    SvgElement* createElementFromDom(const QDomElement& domElement);

    QList<SvgElement*> mElements;
    QRectF mViewBox;
    SvgElement* m_rootElement = nullptr;  // 根元素指针
    QString mTitle;
    QString mDescription;
    bool mIsValid = false;
};

#endif // SVGDocument_H

