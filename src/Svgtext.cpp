#include "SvgText.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QFontMetricsF>

SvgText::SvgText(const QString& id)
    : SvgElement(TypeText, id)
{}

void SvgText::draw(SvgRenderer* renderer) const
{
    if (!renderer || !renderer->painter()) return;

    QPainter* painter = renderer->painter();
    const SvgStyle& style = this->style();
    style.applyToPainter(painter);

    // 绘制文本（SVG文本默认基线对齐，简化处理）
    painter->drawText(mPosition, mText);
}

QRectF SvgText::boundingBox() const
{
    // 使用正确的参数组合计算文本边界框
    QFontMetricsF fm{QFont()};

    // 1. 先获取文本自身的边界框（相对于原点）
    QRectF textBbox = fm.boundingRect(mText);

    // 2. 将边界框平移到文本的实际位置(mPosition)
    textBbox.translate(mPosition);

    return textBbox;
}
