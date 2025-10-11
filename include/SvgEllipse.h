#ifndef SVG_ELLIPSE_H
#define SVG_ELLIPSE_H

#include "SvgElement.h"
#include <QRectF>

class SvgEllipse : public SvgElement {
public:
    SvgEllipse() : SvgElement(TypeShape) {}  // 归类为“图形元素”

    // 核心属性：椭圆中心(cx,cy)、x轴半径rx、y轴半径ry
    void setCx(qreal cx) { mCx = cx; }
    void setCy(qreal cy) { mCy = cy; }
    void setRx(qreal rx) { mRx = rx; }
    void setRy(qreal ry) { mRy = ry; }

    qreal cx() const { return mCx; }   // 用于访问mCx
    qreal cy() const { return mCy; }   // 用于访问mCy
    qreal rx() const { return mRx; }   // 用于访问mRx
    qreal ry() const { return mRy; }   // 用于访问mRy

    // 绘制逻辑
    void draw(SvgRenderer* renderer) const override;

private:
    qreal mCx = 0;
    qreal mCy = 0;
    qreal mRx = 0;
    qreal mRy = 0;
};

#endif // SVG_ELLIPSE_H

