#include "SvgPolyline.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QDebug>

void SvgPolyline::draw(SvgRenderer* renderer) const {
    if (!renderer || !renderer->painter() || mPoints.isEmpty()) return;
    QPainter* painter = renderer->painter();

    // 应用样式（填充/描边均有效）
    const SvgStyle& style = this->style();
    style.applyToPainter(painter, false);

    // 绘制折线（不闭合）
    painter->drawPolyline(mPoints);
    qDebug() << "绘制折线：顶点数量=" << mPoints.size();
}
