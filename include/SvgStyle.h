#ifndef SVGSTYLE_H
#define SVGSTYLE_H

#include "SvgPen.h"
#include "SvgBrush.h"
#include <QString>
#include <QFont>

class SvgStyle
{
public:
    SvgStyle ();
    SvgStyle(const SvgStyle& other);
    SvgStyle& operator=(const SvgStyle& other);
    ~SvgStyle();

    QString textAnchor() const { return mTextAnchor; }

    QColor fill() const { return mFill; }
    QColor stroke() const { return mStroke; }
    qreal strokeWidth() const { return mStrokeWidth; }

    const QString& fontFamily() const { return mFontFamily; }
    qreal fontSize() const { return mFontSize; }

    SvgPen* pen() const { return mPen; }
    void setPen(SvgPen* pen);

    SvgBrush* brush() const { return mBrush; }
    void setBrush(SvgBrush* brush);

    void applyToPainter(QPainter* painter, bool isText) const;

    void parseStyleString(const QString& styleStr);
    void parseAttribute(const QString& name, const QString& value);

    // 合并样式：用other的属性覆盖当前未设置的属性
    void merge(const SvgStyle& other);

    // 设置属性的方法（根据实际需求补充）
    void setFill(const QColor& fill) { mFill = fill; }
    void setStroke(const QColor& stroke) { mStroke = stroke; }
    void setStrokeWidth(qreal width) { mStrokeWidth = width; }

    // 判断属性是否有效
    bool hasFill() const { return mFill.isValid(); }
    bool hasStroke() const { return mStroke.isValid(); }
    bool hasStrokeWidth() const { return mStrokeWidth >= 0; }

private:
    SvgPen* mPen;
    SvgBrush* mBrush;
    QColor mFill;         // 填充颜色
    QColor mStroke;       // 描边颜色
    qreal mStrokeWidth;   // 描边宽度
    QString mFontFamily;  // 字体名称
    qreal mFontSize;      // 字体大小（像素）
    QString mTextAnchor;

    void copyFrom(const SvgStyle& other);
    void clear();
};

#endif // SVGSTYLE_H
