#include "SvgCircle.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QDebug>

SvgCircle::SvgCircle(const QString& id)
    : SvgElement(TypeShape, id)
{}

void SvgCircle::draw(SvgRenderer* renderer) const
{
    if (!renderer || !renderer->painter()) return;
    QPainter* painter = renderer->painter();

    const SvgStyle& style = this->style();
    style.applyToPainter(painter, false);

    // 修复：使用正确的成员变量mCenter和mRadius
    qDebug() << "绘制圆形：中心(" << mCenter.x() << "," << mCenter.y() << "), 半径=" << mRadius
             << "填充=" << style.fill().name()
             << "描边=" << style.stroke().name()
             << "实际描边宽度=" << painter->pen().widthF();

    // 绘制圆形（使用正确的中心和半径）
    QRectF circleRect(
        mCenter.x() - mRadius,  // 左上角x
        mCenter.y() - mRadius,  // 左上角y
        mRadius * 2,            // 宽
        mRadius * 2             // 高
        );
    painter->drawEllipse(circleRect);
}

QRectF SvgCircle::boundingBox() const
{
    // 边界框计算保持不变（已使用正确的mCenter和mRadius）
    return QRectF(
        mCenter.x() - mRadius,
        mCenter.y() - mRadius,
        2 * mRadius,
        2 * mRadius
        );
}
