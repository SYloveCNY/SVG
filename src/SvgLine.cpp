#include "SvgLine.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QDebug>

void SvgLine::draw(SvgRenderer* renderer) const {
    if (!renderer || !renderer->painter()) return;
    QPainter* painter = renderer->painter();

    // 应用样式（填充对直线无效，描边有效）
    const SvgStyle& style = this->style();
    style.applyToPainter(painter, false);

    // 绘制直线
    QLineF line(mX1, mY1, mX2, mY2);
    painter->drawLine(line);
    qDebug() << "绘制直线：从(" << mX1 << "," << mY1 << ")到(" << mX2 << "," << mY2 << ")";
}
