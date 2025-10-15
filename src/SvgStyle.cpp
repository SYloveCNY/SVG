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
    mBrush(new SvgSolidBrush(Qt::black)),
    // 初始化核心样式属性（符合SVG规范默认值）
    mFill(Qt::black),               // 默认填充黑色
    mStroke(Qt::transparent),       // 默认描边透明（无描边）
    mStrokeWidth(-1),               // 默认描边宽度1.0
    mFontFamily("Arial"),           // 默认字体
    mFontSize(16.0)                 // 默认字体大小（像素）
{
}

SvgStyle::SvgStyle(const SvgStyle& other)
    : mPen(nullptr),
    mBrush(nullptr),
    mFill(other.mFill),
    mStroke(other.mStroke),
    mStrokeWidth(other.mStrokeWidth),
    mFontFamily(other.mFontFamily),
    mFontSize(other.mFontSize)
{
    copyFrom(other); // 深拷贝mPen和mBrush
}

SvgStyle& SvgStyle::operator=(const SvgStyle& other) {
    if (this != &other) {  // 避免自赋值
        clear();          // 先删除自身的mPen和mBrush，防止内存泄漏
        // 复制基础类型成员
        mFill = other.mFill;
        mStroke = other.mStroke;
        mStrokeWidth = other.mStrokeWidth;
        mFontFamily = other.mFontFamily;
        mFontSize = other.mFontSize;
        // 深拷贝mPen和mBrush
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

void SvgStyle::parseStyleString(const QString& styleStr)
{
    QStringList props = styleStr.split(';');
    foreach (const QString& prop, props) {
        QStringList keyValue = prop.split(':');
        if (keyValue.size() == 2) {
            parseAttribute(keyValue[0].trimmed(), keyValue[1].trimmed());
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
        QString numStr = value.trimmed();
        QRegularExpression numRegex("(\\d+(\\.\\d+)?)");
        QRegularExpressionMatch match = numRegex.match(numStr);
        if (match.hasMatch()) {  // 替代indexIn() != -1
            bool ok;
            qreal width = match.captured(1).toDouble(&ok);  // 替代cap(1)
            if (ok && width >= 0) {
                mStrokeWidth = width;
                // 新增：检查描边宽度是否远超合理值（如超过100，或后续结合图形尺寸判断）
                if (mStrokeWidth > 100) {
                    qWarning() << "警告：描边宽度过大（" << mStrokeWidth << "），可能覆盖填充色，建议检查SVG";
                    // 可选：限制最大描边宽度（如设为20）
                    mStrokeWidth = 5;
                }
                qDebug() << "描边宽度解析成功：" << mStrokeWidth;
            } else {
                mStrokeWidth = 1.0;
            }
        } else {
            mStrokeWidth = 1.0;
        }
    } else if (name == "font-family") {
        mFontFamily = value;
    } else if (name == "font-size") {
        mFontSize = value.toDouble();
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

// 核心：合并样式（自身未设置的属性用other的属性覆盖）
void SvgStyle::merge(const SvgStyle& other) {
    if (!hasFill() && other.hasFill()) {
        mFill = other.mFill;
    }
    if (!hasStroke() && other.hasStroke()) {
        mStroke = other.mStroke;
    }
    if (!hasStrokeWidth() && other.hasStrokeWidth()) {
        mStrokeWidth = other.mStrokeWidth;
    }
}

// 应用样式到画笔
void SvgStyle::applyToPainter(QPainter* painter) const {
    // 设置填充（画刷）
    if (hasFill()) {
        painter->setBrush(QBrush(mFill));
    } else {
        painter->setBrush(Qt::NoBrush); // 无填充
    }

    // 设置描边（画笔）
    if (hasStroke() && hasStrokeWidth()) {
        QPen pen(mStroke, mStrokeWidth);
        painter->setPen(pen);
    } else {
        painter->setPen(Qt::NoPen); // 无描边
    }
}
