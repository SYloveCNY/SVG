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

private:
    bool parseSvgElement(const QDomElement& domElement);
    SvgElement* createElementFromDom(const QDomElement& domElement);

    QList<SvgElement*> mElements;
    QRectF mViewBox;
    QString mTitle;
    QString mDescription;
};

#endif // SVGDocument_H

