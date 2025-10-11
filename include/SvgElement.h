#ifndef SVGELEMENT_H
#define SVGELEMENT_H

#include <QString>
#include <QPointF>
#include <QRectF>
#include "SvgTransform.h"
#include "SvgStyle.h"

class SvgRenderer;

class SvgElement
{
public:
    enum ElementType {
        TypeShape,
        TypeText,
        TypeClipping,
        TypeGroup
    };

    explicit SvgElement(ElementType type, const QString& id = "");
    virtual ~SvgElement();

    ElementType type() const;
    QString id() const;
    void setId(const QString& id);

    virtual void setTransform(const SvgTransform& transform);
    const SvgTransform& transform() const;

    virtual void setStyle(const SvgStyle& style);
    virtual const SvgStyle& style() const;

    virtual void draw(SvgRenderer* renderer) const = 0;

    virtual QRectF boundingBox() const;
    virtual void setBoundingBox(const QRectF& bbox);


protected:
    ElementType mType;
    QString mId;
    SvgTransform mTransform;
    SvgStyle mStyle;
    QRectF mBoundingBox;
};

#endif // SVGELEMENT_H
