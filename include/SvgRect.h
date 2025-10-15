#ifndef SVGRECT_H
#define SVGRECT_H

#include "SvgElement.h"
#include <QRectF>

class SvgRect : public SvgElement {
public:
    SvgRect() : SvgElement(TypeShape), mX(0), mY(0), mWidth(0), mHeight(0), mRx(0), mRy(0) {}

    // 原有属性的getter/setter
    qreal x() const { return mX; }
    void setX(qreal x) { mX = x; }

    qreal y() const { return mY; }
    void setY(qreal y) { mY = y; }

    qreal width() const { return mWidth; }
    void setWidth(qreal width) { mWidth = width; }

    qreal height() const { return mHeight; }
    void setHeight(qreal height) { mHeight = height; }

    // 圆角属性的getter/setter
    qreal rx() const { return mRx; }
    void setRx(qreal rx) { mRx = rx; }

    qreal ry() const { return mRy; }
    void setRy(qreal ry) { mRy = ry; }

    // 只保留一个draw函数声明
    void draw(SvgRenderer* renderer) const override;

    QRectF boundingBox() const override {
        return QRectF(mX, mY, mWidth, mHeight);
    }

private:
    qreal mX;
    qreal mY;
    qreal mWidth;
    qreal mHeight;
    qreal mRx;
    qreal mRy;
};

#endif // SVGRECT_H
