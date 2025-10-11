#ifndef SVG_POLYGON_H
#define SVG_POLYGON_H

#include "SvgElement.h"
#include <QPolygonF>

class SvgPolygon : public SvgElement {
public:
    SvgPolygon() : SvgElement(TypeShape) {}

    // 设置多边形的顶点列表（自动闭合）
    void setPoints(const QPolygonF& points) { mPoints = points; }
    const QPolygonF& points() const { return mPoints; }

    // 绘制多边形
    void draw(SvgRenderer* renderer) const override;

private:
    QPolygonF mPoints;  // 存储多边形的所有顶点
};

#endif // SVG_POLYGON_H
