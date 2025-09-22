#include "SvgElement.h"
#include "SvgRenderer.h"

// 构造函数
SvgElement::SvgElement(ElementType type, const QString& id)
    : mType(type), mId(id)
{
}

// 析构函数
SvgElement::~SvgElement()
{
}

// 获取元素类型
SvgElement::ElementType SvgElement::type() const
{
    return mType;
}

// 获取ID
QString SvgElement::id() const
{
    return mId;
}

// 设置ID
void SvgElement::setId(const QString& id)
{
    mId = id;
}

// 设置变换
void SvgElement::setTransform(const SvgTransform& transform)
{
    mTransform = transform;
}

// 获取变换
const SvgTransform& SvgElement::transform() const
{
    return mTransform;
}

// 设置样式
void SvgElement::setStyle(const SvgStyle& style)
{
    mStyle = style;
}

// 获取样式
const SvgStyle& SvgElement::style() const
{
    return mStyle;
}

// 获取边界框
QRectF SvgElement::boundingBox() const
{
    return mBoundingBox;
}

// 设置边界框
void SvgElement::setBoundingBox(const QRectF& bbox)
{
    mBoundingBox = bbox;
}
