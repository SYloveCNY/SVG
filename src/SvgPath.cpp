#include "SvgPath.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QDebug>

void SvgPath::draw(SvgRenderer* renderer) const {
    qDebug() << "[SvgPath] 开始绘制路径，路径段数量：" << mPath.elementCount();
    if (mPath.elementCount() == 0) {
        qDebug() << "[SvgPath] 路径为空，跳过绘制";
        return;
    }

    if (!renderer || !renderer->painter()) return;
    QPainter* painter = renderer->painter();
    painter->save();

    // 同样修复样式合并优先级
    SvgStyle parentStyle = renderer->currentStyle();
    SvgStyle mergedStyle = mStyle; // 自身样式优先
    mergedStyle.merge(parentStyle);
    mergedStyle.applyToPainter(painter);

    // 绘制路径（关键：调用drawPath）
    QRectF logicRect = mPath.boundingRect();
    QRectF physicalRect = renderer->currentTransform().mapRect(logicRect);
    painter->drawPath(mPath); // 绘制解析后的路径

    painter->restore();
    qDebug() << "[SvgPath] 路径绘制完成";
}
