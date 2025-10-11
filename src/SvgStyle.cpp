#include "SvgStyle.h"
#include "SvgPen.h"
#include "SvgBrush.h"
#include <QPainter>
#include <QStringList>
#include <QColor>
#include <QRegularExpression>
#include <QDebug>

SvgStyle::SvgStyle()
    : mPen(new SvgPen()),
    mBrush(new SvgSolidBrush(Qt::black))
{
}

SvgStyle& SvgStyle::operator=(const SvgStyle& other) {
    if (this != &other) {  // 避免自赋值
        mFill = other.mFill;
        mStroke = other.mStroke;
        mStrokeWidth = other.mStrokeWidth;
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

    // 打印样式信息（关键调试）
        qDebug() << "应用样式 - 填充：" << mFill.name()
        << "描边：" << mStroke.name()
        << "宽度：" << mStrokeWidth;

    // 设置填充
    painter->setBrush(mFill);

    // 设置描边
    QPen pen(mStroke, mStrokeWidth);
    painter->setPen(pen);
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
    qDebug() << "解析样式属性：" << name << "=" << value;

    if (name == "fill") {
        // 1. 优先处理SVG规范中的"none"（无填充，透明）
        QString lowerValue = value.trimmed().toLower();
        if (lowerValue == "none") {
            mFill = Qt::transparent;
            qDebug() << "填充设置为无（透明）";
            return;
        }
        // 2. 解析正常颜色（支持命名色、十六进制等）
        QColor color(value);
        if (color.isValid()) {
            mFill = color;
            qDebug() << "填充颜色解析成功：" << mFill.name();
        } else {
            qDebug() << "填充颜色无效，使用默认透明";
            mFill = Qt::transparent;  // 无效时建议默认透明而非黑色
        }
    } else if (name == "stroke") {
        // 同样处理stroke="none"（无描边）
        QString lowerValue = value.trimmed().toLower();
        if (lowerValue == "none") {
            mStroke = Qt::transparent;
            qDebug() << "描边设置为无（透明）";
            return;
        }
        // 解析正常描边颜色
        QColor color(value);
        if (color.isValid()) {
            mStroke = color;
            qDebug() << "描边颜色解析成功：" << mStroke.name();
        } else {
            qDebug() << "描边颜色无效，使用默认透明";
            mStroke = Qt::transparent;
        }
    } else if (name == "stroke-width") {
        // 提取纯数字（处理如"3px"、"5.5em"等带单位的情况）
        QString numStr = value.trimmed();
        QRegularExpression numRegex("(\\d+(\\.\\d+)?)");  // 匹配数字部分
        QRegularExpressionMatch match = numRegex.match(numStr);

        if (match.hasMatch()) {  // 检查是否匹配成功
            bool ok;
            qreal width = match.captured(1).toDouble(&ok);  // 获取第一个捕获组
            if (ok && width >= 0) {
                mStrokeWidth = width;
                qDebug() << "描边宽度解析成功（去单位）：" << mStrokeWidth;
            } else {
                mStrokeWidth = 1.0;  // 默认1
            }
        } else {
            mStrokeWidth = 1.0;
        }
    }
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
