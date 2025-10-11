#ifndef SVG_RENDERER_H
#define SVG_RENDERER_H

#include <QPainter>
#include <QRectF>
#include <QTransform>
#include <QStack>  // 修正：使用QStack而非QList

class SvgDocument;
class SvgElement;
class SvgTransform;

class SvgRenderer
{
public:
    SvgRenderer();
    ~SvgRenderer();

    void setPainter(QPainter* painter);
    QPainter* painter() const;

    void setViewBox(const QRectF& viewBox);
    QRectF viewBox() const;

    void pushTransform(const SvgTransform& transform);
    void popTransform();
    QTransform currentTransform() const;

    void render(const SvgDocument* document, QPainter* painter, const QRectF& viewport);

private:
    QPainter* mPainter;
    QRectF mViewBox;
    // 修正3：使用QStack存储变换矩阵
    QStack<QTransform> mTransformStack;
    void renderElement(const SvgElement* element, QPainter* painter);
};

#endif // SVG_RENDERER_H
