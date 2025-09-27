#include "SvgCircle.h"
#include "SvgRenderer.h"
#include <QPainter>

SvgCircle::SvgCircle(const QString& id)
    : SvgElement(TypeShape, id)
{}

void SvgCircle::draw(SvgRenderer* renderer) const
{
    if (!renderer || !renderer->painter()) return;

    QPainter* painter = renderer->painter();
    const SvgStyle& style = this->style();
    style.applyToPainter(painter);

    // 绘制圆形（椭圆的特殊情况，宽高=2*半径）
    painter->drawEllipse(mCenter, mRadius, mRadius);
}

QRectF SvgCircle::boundingBox() const
{
    // 圆形边界框：左上角(cx-r, cy-r)，宽高2r
    return QRectF(
        mCenter.x() - mRadius,
        mCenter.y() - mRadius,
        2 * mRadius,
        2 * mRadius
        );
}
