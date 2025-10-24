#include "SvgText.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QFontMetricsF>
#include <QDebug>

SvgText::SvgText(const QString& id)
    : SvgElement(TypeText, id)
{}

void SvgText::draw(SvgRenderer* renderer) const
{
    if (!renderer || !renderer->painter()) {
        qWarning() << "无效的渲染器或画笔，无法绘制文本";
        return;
    }

    QPainter* painter = renderer->painter();
    const SvgStyle& style = this->style();

    painter->save(); // 保存画笔状态

    // 1. 设置字体
    QFont font = painter->font();
    if (!style.fontFamily().isEmpty()) {
        font.setFamily(style.fontFamily());
    }
    qreal fontSize = style.fontSize() > 0 ? style.fontSize() : 12;
    font.setPointSizeF(fontSize);
    painter->setFont(font);

    // 2. 应用文本样式（fill为文本颜色，stroke为描边）
    QPen textPen;
    if (style.hasStroke() && style.strokeWidth() > 0) {
        textPen.setColor(style.stroke());
        textPen.setWidthF(style.strokeWidth());
    } else {
        textPen.setWidthF(0);
    }
    textPen.setColor(style.hasFill() ? style.fill() : Qt::black); // 优先使用fill
    painter->setPen(textPen);
    painter->setBrush(Qt::NoBrush);

    // 3. 处理text-anchor对齐（调整逻辑坐标x）
    QFontMetricsF fm(font);
    qreal textWidth = fm.horizontalAdvance(mText);
    QPointF logicPos = mPosition;

    if (style.textAnchor() == "middle") {
        logicPos.rx() -= textWidth / 2;
    } else if (style.textAnchor() == "end") {
        logicPos.rx() -= textWidth;
    }

    // 4. 直接使用逻辑坐标绘制（依赖画笔已有变换自动转换为物理坐标）
    // 关键修复：删除手动计算physicalPos的逻辑，使用逻辑坐标
    painter->drawText(logicPos, mText);

    painter->restore(); // 恢复画笔状态
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
