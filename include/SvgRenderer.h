#ifndef SVG_RENDERER_H
#define SVG_RENDERER_H

#include "SvgStyle.h"
#include <QPainter>
#include <QRectF>
#include <QTransform>
#include <QStack>

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

    // 1. 删除重复的声明（只保留一个currentTransform）
    const QTransform& currentTransform() const { return mCurrentTransform; }

    void render(const SvgDocument* document, QPainter* painter, const QRectF& viewport);

    // 获取当前样式（用于继承）
    const SvgStyle& currentStyle() const { return mCurrentStyle; }

    // 应用变换（用于嵌套元素）
    void applyTransform(const QTransform& transform) {
        mCurrentTransform *= transform;
    }

private:
    QPainter* mPainter;
    QRectF mViewBox;
    QStack<QTransform> mTransformStack;
    SvgStyle mCurrentStyle;   // 当前样式（用于继承）
    // 2. 添加缺失的mCurrentTransform成员变量
    QTransform mCurrentTransform;  // 存储当前变换矩阵
    void renderElement(const SvgElement* element, QPainter* painter);

};

#endif // SVG_RENDERER_H
