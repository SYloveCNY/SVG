#include "SvgElement.h"
#include "SvgRenderer.h"
#include <QDebug>

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
    // 打印地址，确认mStyle是成员变量
    qDebug() << "setStyle - 成员mStyle地址：" << &mStyle;
    qDebug() << "setStyle - 传入style地址：" << &style;

    mStyle = style;

    qDebug() << "setStyle后 - mStyle填充：" << mStyle.fill().name()
             << "描边：" << mStyle.stroke().name()
             << "宽度：" << mStyle.strokeWidth();

}

// 获取样式
const SvgStyle& SvgElement::style() const
{
    qDebug() << "style()返回 - mStyle填充：" << mStyle.fill().name()
             << "描边：" << mStyle.stroke().name()
             << "宽度：" << mStyle.strokeWidth();

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
