#include "SvgRenderer.h"
#include "SvgDocument.h"
#include "SvgElement.h"
#include "SvgGroup.h"
#include "SvgStyle.h"
#include "SvgTransform.h"
#include "SvgRect.h"
#include <QPainter>
#include <QStack>
#include <QDebug>

SvgRenderer::SvgRenderer()
    : mPainter(nullptr)
{
}

SvgRenderer::~SvgRenderer()
{
}

void SvgRenderer::setPainter(QPainter* painter)
{
    mPainter = painter;
}

QPainter* SvgRenderer::painter() const
{
    return mPainter;
}

void SvgRenderer::setViewBox(const QRectF& viewBox)
{
    mViewBox = viewBox;
}

QRectF SvgRenderer::viewBox() const
{
    return mViewBox;
}

void SvgRenderer::pushTransform(const SvgTransform& transform)
{
    if (mPainter) {
        mTransformStack.push(mPainter->transform());
        mPainter->setTransform(transform.toQTransform(), true);
    }
}

void SvgRenderer::popTransform()
{
    if (mPainter && !mTransformStack.isEmpty()) {
        mPainter->setTransform(mTransformStack.pop());
    }
}

QTransform SvgRenderer::currentTransform() const
{
    if (mPainter) {
        return mPainter->transform();
    }
    return QTransform();
}

void SvgRenderer::render(const SvgDocument* document, QPainter* painter, const QRectF& viewport) {
    if (!document || !painter) {
        qDebug() << "render失败：document或painter为空";
        return;
    }

    painter->save();
    setPainter(painter);

    // 1. 关键修复1：从document获取viewBox（而非渲染器自身的mViewBox）
    QRectF svgLogicRect = document->viewBox();  // 正确来源：SVG文档的viewBox
    QRectF windowPhysRect = viewport;

    // 2. 关键修复2：强制处理无效viewBox（宽高<=0的情况）
    if (svgLogicRect.width() <= 0 || svgLogicRect.height() <= 0) {
        // 用默认值200x200避免除以0，或根据元素计算（推荐后者）
        svgLogicRect = QRectF(0, 0, 200, 200);
        qDebug() << "警告：viewBox无效，使用默认值：" << svgLogicRect;
    }

    qDebug() << "SVG逻辑范围（viewBox）：" << svgLogicRect;
    qDebug() << "窗口物理尺寸：" << windowPhysRect;

    // 3. 计算缩放和偏移（确保分母不为0，因上面已处理）
    qreal scaleX = windowPhysRect.width() / svgLogicRect.width();
    qreal scaleY = windowPhysRect.height() / svgLogicRect.height();
    qreal scale = qMin(scaleX, scaleY);  // 等比例缩放，避免图形变形

    qreal offsetX = windowPhysRect.x() + (windowPhysRect.width() - svgLogicRect.width() * scale) / 2;
    qreal offsetY = windowPhysRect.y() + (windowPhysRect.height() - svgLogicRect.height() * scale) / 2;

    // 构建变换矩阵（逻辑坐标→物理坐标）
    QTransform viewTransform;
    viewTransform.translate(offsetX, offsetY);  // 先平移到窗口中心
    viewTransform.scale(scale, scale);          // 再缩放
    viewTransform.translate(-svgLogicRect.x(), -svgLogicRect.y());  // 抵消viewBox原点偏移

    // 应用变换到画笔
    mTransformStack.clear();
    mTransformStack.push(viewTransform);
    painter->setTransform(viewTransform);
    qDebug() << "应用的坐标变换：" << viewTransform;  // 新增日志验证变换是否有效

    // 渲染元素（此时绘制逻辑坐标会自动转换为物理坐标）
    qDebug() << "开始渲染SVG元素，元素数量：" << document->elements().size();
    foreach (const SvgElement* element, document->elements()) {
        renderElement(element, painter);
    }

    painter->restore();
    qDebug() << "mRenderer.render调用完成";
}


void SvgRenderer::renderElement(const SvgElement* element, QPainter* painter) {
    if (!element || !painter) return;

    // 应用元素变换（复用原逻辑）
    const SvgTransform& elemTrans = element->transform();
    if (!elemTrans.isIdentity()) pushTransform(elemTrans);

    // 调用元素的draw方法（虚函数：rect→SvgRect::draw，ellipse→SvgEllipse::draw）
    element->draw(this);  // 关键：多态调用

    // 弹出变换
    if (!elemTrans.isIdentity()) popTransform();
}
