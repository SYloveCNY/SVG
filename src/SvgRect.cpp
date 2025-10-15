#include "SvgRect.h"
#include "SvgRenderer.h"
#include <QPainter>
#include <QDebug>

void SvgRect::draw(SvgRenderer* renderer) const
{
    qDebug () << "[步骤 1] 开始绘制矩形：x=" << mX << "y=" << mY << "w=" << mWidth << "h=" << mHeight;

    // 校验渲染器和画笔有效性
    if (!renderer || !renderer->painter()) {
        qWarning () << "[错误] 渲染器为空！";
        return;
    }
    qDebug () << "[步骤 2] 渲染器有效";

    QPainter* painter = renderer->painter();
    if (!painter) {
        qWarning () << "[错误] 画笔为空！";
        return;
    }
    qDebug () << "[步骤 3] 画笔有效";

    // 保存画笔状态，避免影响后续绘制
    painter->save();
    qDebug () << "[步骤 4] 保存画笔状态成功";

    // 1. 处理样式：合并自身样式与父级样式（自身样式优先）
    SvgStyle parentStyle = renderer->currentStyle();
    SvgStyle mergedStyle = mStyle;
    mergedStyle.merge(parentStyle);
    qDebug () << "[步骤 5] 样式合并成功：填充色 =" << mergedStyle.fill ().name ()<< "描边色 =" << mergedStyle.stroke ().name ()<< "描边宽度 =" << mergedStyle.strokeWidth ();

    // 2. 应用合并后的样式到画笔
    mergedStyle.applyToPainter(painter);
    qDebug () << "[步骤 6] 样式合并成功：填充色 =" << mergedStyle.fill ().name ()
             << "描边色 =" << mergedStyle.stroke ().name ()
             << "描边宽度 =" << mergedStyle.strokeWidth ();

    // 3. 坐标转换：将逻辑坐标转换为物理屏幕坐标
    QRectF logicRect(mX, mY, mWidth, mHeight);
    QRectF physicalRect = renderer->currentTransform().mapRect(logicRect);
    qDebug() << "矩形物理坐标：" << physicalRect;
    qDebug () << "[步骤 7] 坐标转换完成：逻辑坐标 =" << logicRect << "物理坐标 =" << physicalRect;

    // 4. 绘制矩形（使用转换后的物理坐标）
    painter->drawRect(physicalRect);
    qDebug () << "[步骤 8] 矩形绘制完成";

    // 恢复画笔状态
    painter->restore();
    qDebug () << "[步骤 9] 恢复画笔状态成功";
}

