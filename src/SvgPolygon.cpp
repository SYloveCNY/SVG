#include "SvgPolygon.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QDebug>

void SvgPolygon::draw(SvgRenderer* renderer) const {
    if (!renderer || !renderer->painter()) return;
    QPainter* painter = renderer->painter();

    // 1. 打印顶点坐标（修复：将mapPoint改为map）
    qDebug() << "绘制多边形：原始顶点数量=" << mPoints.size();
    for (int i = 0; i < mPoints.size(); ++i) {
        // QTransform的正确方法是map()，而非mapPoint()
        QPointF physPoint = renderer->currentTransform().map(mPoints[i]);
        qDebug() << "顶点" << i << "：逻辑坐标(" << mPoints[i].x() << "," << mPoints[i].y()
                 << ") → 物理坐标(" << physPoint.x() << "," << physPoint.y() << ")";
    }

    // 2. 应用样式
    const SvgStyle& style = this->style();
    style.applyToPainter(painter);
    qDebug() << "多边形样式：填充=" << style.fill().name()
             << "描边=" << style.stroke().name() << "宽度=" << style.strokeWidth();

    // 3. 应用元素自身的transform（若有）
    const SvgTransform& elemTrans = this->transform();
    if (!elemTrans.isIdentity()) {
        painter->save();
        painter->setTransform(elemTrans.toQTransform(), true);  // 叠加当前变换
    }

    // 4. 绘制多边形
    painter->drawPolygon(mPoints);

    // 5. 恢复变换
    if (!elemTrans.isIdentity()) {
        painter->restore();
    }
}
