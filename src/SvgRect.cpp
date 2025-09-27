#include "SvgRect.h"
#include "SvgRenderer.h"
#include <QPainter>

SvgRect::SvgRect(const QString& id)
    : SvgElement(TypeShape, id)
{}

void SvgRect::draw(SvgRenderer* renderer) const
{
    if (!renderer || !renderer->painter()) return;

    QPainter* painter = renderer->painter();
    // 应用当前元素的样式
    const SvgStyle& style = this->style();
    style.applyToPainter(painter);

    // 绘制矩形
    painter->drawRect(QRectF(mX, mY, mWidth, mHeight));
}

QRectF SvgRect::boundingBox() const
{
    // 矩形的边界框就是自身
    return QRectF(mX, mY, mWidth, mHeight);
}
