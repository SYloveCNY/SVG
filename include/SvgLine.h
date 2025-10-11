#ifndef SVG_LINE_H
#define SVG_LINE_H

#include "SvgElement.h"
#include <QLineF>

class SvgLine : public SvgElement {
public:
    SvgLine() : SvgElement(TypeShape) {}

    // 直线的起点和终点
    void setX1(qreal x1) { mX1 = x1; }
    void setY1(qreal y1) { mY1 = y1; }
    void setX2(qreal x2) { mX2 = x2; }
    void setY2(qreal y2) { mY2 = y2; }

    qreal x1() const { return mX1; }   // 用于访问mX1
    qreal y1() const { return mY1; }   // 用于访问mY1
    qreal x2() const { return mX2; }   // 用于访问mX2
    qreal y2() const { return mY2; }   // 用于访问mY2

    // 绘制直线
    void draw(SvgRenderer* renderer) const override;

private:
    qreal mX1 = 0;  // 起点x
    qreal mY1 = 0;  // 起点y
    qreal mX2 = 0;  // 终点x
    qreal mY2 = 0;  // 终点y
};

#endif // SVG_LINE_H

