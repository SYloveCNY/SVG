#ifndef SVG_POLYLINE_H
#define SVG_POLYLINE_H

#include "SvgElement.h"
#include <QPolygonF>

class SvgPolyline : public SvgElement {
public:
    SvgPolyline() : SvgElement(TypeShape) {}

    // 设置折线的顶点列表
    void setPoints(const QPolygonF& points) { mPoints = points; }
    const QPolygonF& points() const { return mPoints; }

    // 绘制折线
    void draw(SvgRenderer* renderer) const override;

private:
    QPolygonF mPoints;  // 存储折线的所有顶点
};

#endif // SVG_POLYLINE_H

