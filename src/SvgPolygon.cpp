#include "SvgPolygon.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QDebug>

void SvgPolygon::draw(SvgRenderer* renderer) const {
    if (!renderer || !renderer->painter() || mPoints.size() < 3) return;  // 至少3个顶点
    QPainter* painter = renderer->painter();

    // 应用样式（填充/描边均有效）
    const SvgStyle& style = this->style();
    style.applyToPainter(painter);

    // 绘制多边形（自动闭合首尾顶点）
    painter->drawPolygon(mPoints);
    qDebug() << "绘制多边形：顶点数量=" << mPoints.size();
}
