#include "SvgStyle.h"
#include "SvgPen.h"
#include "SvgBrush.h"
#include <QPainter>
#include <QStringList>
#include <QColor>

SvgStyle::SvgStyle()
    : mPen(new SvgPen()),
    mBrush(new SvgSolidBrush(Qt::black))
{
}

SvgStyle::SvgStyle(const SvgStyle& other)
    : mPen(nullptr),
    mBrush(nullptr)
{
    copyFrom(other);
}

SvgStyle& SvgStyle::operator=(const SvgStyle& other)
{
    if (this != &other) {
        clear();
        copyFrom(other);
    }
    return *this;
}

SvgStyle::~SvgStyle()
{
    clear();
}

void SvgStyle::setPen(SvgPen* pen)
{
    if (mPen != pen) {
        delete mPen;
        mPen = pen;
    }
}

void SvgStyle::setBrush(SvgBrush* brush)
{
    if (mBrush != brush) {
        delete mBrush;
        mBrush = brush;
    }
}

void SvgStyle::applyToPainter(QPainter* painter) const
{
    if (!painter) return;

    if (mPen) {
        painter->setPen(mPen->toQPen());
    }

    if (mBrush) {
        painter->setBrush(mBrush->toQBrush());
    }
}

void SvgStyle::parseStyleString(const QString& styleStr)
{
    QStringList declarations = styleStr.split(';', Qt::SkipEmptyParts);
    foreach (const QString& decl, declarations) {
        QStringList parts = decl.split(':', Qt::SkipEmptyParts);
        if (parts.size() == 2) {
            QString name = parts[0].trimmed();
            QString value = parts[1].trimmed();
            parseAttribute(name, value);
        }
    }
}

void SvgStyle::parseAttribute(const QString& name, const QString& value)
{
    if (name == "fill") {
        if (value == "none") {
            if (mBrush->type() == SvgBrush::Solid) {
                static_cast<SvgSolidBrush*>(mBrush)->setColor(Qt::transparent);
            }
        } else {
            // 假设是颜色值
            setBrush(new SvgSolidBrush(QColor(value)));
        }
    } else if (name == "stroke") {
        if (value == "none") {
            mPen->setColor(Qt::transparent);
        } else {
            mPen->setColor(QColor(value));
        }
    } else if (name == "stroke-width") {
        mPen->setWidth(value.toDouble());
    }
    // 可以添加更多属性解析
}

void SvgStyle::copyFrom(const SvgStyle& other)
{
    if (other.mPen) {
        mPen = other.mPen->clone();
    }
    if (other.mBrush) {
        mBrush = other.mBrush->clone();
    }
}

void SvgStyle::clear()
{
    delete mPen;
    mPen = nullptr;

    delete mBrush;
    mBrush = nullptr;
}
