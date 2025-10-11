#include "SvgEllipse.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QDebug>

void SvgEllipse::draw(SvgRenderer* renderer) const {
    if (!renderer || !renderer->painter()) return;
    QPainter* painter = renderer->painter();

    // 应用样式（复用SvgStyle）
    const SvgStyle& style = this->style();
    style.applyToPainter(painter);

    // 椭圆的绘制：用QRectF表示外接矩形（中心偏移+半径）
    QRectF ellipseRect(mCx - mRx, mCy - mRy, mRx * 2, mRy * 2);
    painter->drawEllipse(ellipseRect);
    qDebug() << "绘制椭圆：中心(" << mCx << "," << mCy << "), 半径(" << mRx << "," << mRy << ")";
}
