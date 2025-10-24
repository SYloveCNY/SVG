#include "SvgPath.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QDebug>

void SvgPath::draw(SvgRenderer* renderer) const {
    if (!renderer || !renderer->painter()) return;
    QPainter* painter = renderer->painter();

    // 应用样式
    const SvgStyle& style = this->style();
    style.applyToPainter(painter, false);
    qDebug() << "绘制路径：路径段数量=" << mPath.elementCount()
             << "填充：" << style.fill().name() << "描边：" << style.stroke().name();

    // 绘制路径
    painter->drawPath(mPath);
}
