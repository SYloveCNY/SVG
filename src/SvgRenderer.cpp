#include "SvgRenderer.h"
#include "SvgDocument.h"
#include "SvgElement.h"
#include "SvgStyle.h"
#include "SvgTransform.h"
#include <QPainter>
#include <QStack>

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

void SvgRenderer::render(const SvgDocument* document)
{
    if (!mPainter || !document) return;

    mPainter->save();

    QRectF docViewBox = document->viewBox();
    if (!docViewBox.isEmpty()) {
        QRect deviceRect = mPainter->viewport();
        QTransform viewTransform;
        viewTransform.scale(
            deviceRect.width() / docViewBox.width(),
            deviceRect.height() / docViewBox.height()
            );
        viewTransform.translate(-docViewBox.x(), -docViewBox.y());
        mPainter->setTransform(viewTransform, true);
    }

    foreach (const SvgElement* element, document->elements()) {
        renderElement(element);
    }

    mPainter->restore();
}

void SvgRenderer::renderElement(const SvgElement* element)
{
    if (!mPainter || !element) return;

    mPainter->save();

    const SvgTransform& transform = element->transform();
    if (!transform.toQTransform().isIdentity()) {
        mPainter->setTransform(transform.toQTransform(), true);
    }

    const SvgStyle& style = element->style();
    style.applyToPainter(mPainter);

    // 调用const版本的draw方法
    element->draw(this);

    mPainter->restore();
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
