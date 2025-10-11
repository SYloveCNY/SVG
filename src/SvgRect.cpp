#include "SvgRect.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QDebug>

SvgRect::SvgRect(const QString& id)
    : SvgElement(TypeShape, id)
{}

void SvgRect::draw(SvgRenderer* renderer) const
{
    qDebug() << "绘制矩形：x=" << mX << "y=" << mY << "w=" << mWidth << "h=" << mHeight;

    if (!renderer || !renderer->painter()) return;

    QPainter* painter = renderer->painter();
    // 临时强制设置红色填充（忽略原样式，测试用）
    painter->setBrush(Qt::red);  // 红色填充
    painter->setPen(Qt::NoPen);  // 无描边（可选）

    // 转换坐标：使用渲染器的当前变换
    QRectF logicRect(mX, mY, mWidth, mHeight);
    QRectF physicalRect = renderer->currentTransform().mapRect(logicRect);
    qDebug() << "矩形物理坐标：" << physicalRect;

    // 绘制矩形
    painter->drawRect(QRectF(mX, mY, mWidth, mHeight));
}

QRectF SvgRect::boundingBox() const
{
    // 矩形的边界框就是自身
    return QRectF(mX, mY, mWidth, mHeight);
}
