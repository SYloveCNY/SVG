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

    // 保存画笔状态（关键：避免字体/样式影响后续元素）
    painter->save();

    // 1. 解析并设置字体（处理默认值和单位）
    QFont font = painter->font(); // 基于当前默认字体修改
    if (!style.fontFamily().isEmpty()) {
        font.setFamily(style.fontFamily()); // 应用 fontFamily（如"Arial"）
    }
    // 处理字体大小（SVG默认单位为px，需转换为QFont的点大小）
    qreal fontSize = style.fontSize() > 0 ? style.fontSize() : 12; // 默认12px
    font.setPointSizeF(fontSize); // 使用setPointSizeF支持小数大小
    painter->setFont(font);

    // 2. 应用文本样式（重点：SVG文本的fill对应文本颜色）
    // 文本的"fill"是文本颜色，"stroke"是文本描边（可选）
    QPen textPen = painter->pen();
    if (style.hasFill()) {
        textPen.setColor(style.fill()); // 文本颜色=fill属性
    }
    if (style.hasStroke() && style.strokeWidth() > 0) {
        textPen.setColor(style.stroke()); // 文本描边颜色
        textPen.setWidthF(style.strokeWidth()); // 描边宽度
    } else {
        textPen.setWidthF(0); // 无描边
    }
    painter->setPen(textPen);
    // 文本不需要填充画刷（禁用背景填充）
    painter->setBrush(Qt::NoBrush);

    // 3. 处理text-anchor对齐（基于SVG规范的水平对齐）
    QFontMetricsF fm(painter->font());
    qreal textWidth = fm.horizontalAdvance(mText); // 更准确的文本宽度（不含额外边距）
    QPointF logicPos = mPosition; // 原始逻辑坐标（x,y）

    // 根据text-anchor调整x坐标（SVG逻辑：以x为基准点对齐）
    if (style.textAnchor() == "middle") {
        logicPos.rx() -= textWidth / 2; // 居中对齐（x为中心点）
    } else if (style.textAnchor() == "end") {
        logicPos.rx() -= textWidth; // 右对齐（x为终点）
    }
    // "start"对齐（默认）：x为起点，无需调整

    qDebug() << "文本逻辑坐标：" << logicPos;

    // 4. 应用坐标变换（逻辑坐标→物理屏幕坐标）
    QTransform transform = renderer->currentTransform();
    QPointF physicalPos = transform.map(logicPos);
    qDebug() << "文本变换矩阵：" << transform; // 应输出含1.5缩放和25偏移的矩阵
    qDebug() << "文本物理坐标（修正后）：" << physicalPos; // 预期 (400, 45)

    // 5. 绘制文本（使用转换后的物理坐标）
    painter->drawText(physicalPos, mText);

    // 恢复画笔状态（必须：清除当前字体/样式设置）
    painter->restore();
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
