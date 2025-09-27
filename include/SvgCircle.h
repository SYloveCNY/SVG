#ifndef SVGCIRCLE_H
#define SVGCIRCLE_H

#include "SvgElement.h"
#include <QPointF>

class SvgCircle : public SvgElement
{
public:
    explicit SvgCircle(const QString& id = "");
    ~SvgCircle() override = default;

    void draw(SvgRenderer* renderer) const override;
    QRectF boundingBox() const override;

    // 属性访问
    QPointF center() const { return mCenter; }
    void setCenter(const QPointF& center) { mCenter = center; }

    qreal radius() const { return mRadius; }
    void setRadius(qreal radius) { mRadius = radius; }

private:
    QPointF mCenter{0.0, 0.0};
    qreal mRadius = 0.0;
};

#endif // SVGCIRCLE_H
