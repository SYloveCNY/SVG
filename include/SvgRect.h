#ifndef SVGRECT_H
#define SVGRECT_H

#include "SvgElement.h"
#include <QRectF>

class SvgRect : public SvgElement
{
public:
    explicit SvgRect(const QString& id = "");
    ~SvgRect() override = default;

    void draw(SvgRenderer* renderer) const override;
    QRectF boundingBox() const override;

    // 属性访问
    qreal x() const { return mX; }
    void setX(qreal x) { mX = x; }

    qreal y() const { return mY; }
    void setY(qreal y) { mY = y; }

    qreal width() const { return mWidth; }
    void setWidth(qreal width) { mWidth = width; }

    qreal height() const { return mHeight; }
    void setHeight(qreal height) { mHeight = height; }

private:
    qreal mX = 0.0;
    qreal mY = 0.0;
    qreal mWidth = 0.0;
    qreal mHeight = 0.0;
};

#endif // SVGRECT_H
