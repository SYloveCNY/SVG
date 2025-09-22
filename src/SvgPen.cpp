#include "SvgPen.h"

SvgPen::SvgPen()
    : mColor(Qt::black),
    mWidth(1.0),
    mStyle(Qt::SolidLine),
    mCapStyle(Qt::SquareCap),
    mJoinStyle(Qt::MiterJoin)
{
}

SvgPen::SvgPen(const SvgPen& other)
    : mColor(other.mColor),
    mWidth(other.mWidth),
    mStyle(other.mStyle),
    mCapStyle(other.mCapStyle),
    mJoinStyle(other.mJoinStyle),
    mDashPattern(other.mDashPattern)
{
}

SvgPen::~SvgPen()
{
}

// 实现clone方法，创建当前对象的副本
SvgPen* SvgPen::clone() const
{
    return new SvgPen(*this);
}

QPen SvgPen::toQPen() const
{
    QPen pen(mColor, mWidth, mStyle, mCapStyle, mJoinStyle);
    pen.setDashPattern(mDashPattern);
    return pen;
}

QColor SvgPen::color() const
{
    return mColor;
}

void SvgPen::setColor(const QColor& color)
{
    mColor = color;
}

qreal SvgPen::width() const
{
    return mWidth;
}

void SvgPen::setWidth(qreal width)
{
    mWidth = width;
}

Qt::PenStyle SvgPen::style() const
{
    return mStyle;
}

void SvgPen::setStyle(Qt::PenStyle style)
{
    mStyle = style;
}

Qt::PenCapStyle SvgPen::capStyle() const
{
    return mCapStyle;
}

void SvgPen::setCapStyle(Qt::PenCapStyle capStyle)
{
    mCapStyle = capStyle;
}

Qt::PenJoinStyle SvgPen::joinStyle() const
{
    return mJoinStyle;
}

void SvgPen::setJoinStyle(Qt::PenJoinStyle joinStyle)
{
    mJoinStyle = joinStyle;
}

QVector<qreal> SvgPen::dashPattern() const
{
    return mDashPattern;
}

void SvgPen::setDashPattern(const QVector<qreal>& pattern)
{
    mDashPattern = pattern;
}

SvgPen& SvgPen::operator=(const SvgPen& other)
{
    if (this != &other) {
        mColor = other.mColor;
        mWidth = other.mWidth;
        mStyle = other.mStyle;
        mCapStyle = other.mCapStyle;
        mJoinStyle = other.mJoinStyle;
        mDashPattern = other.mDashPattern;
    }
    return *this;
}
