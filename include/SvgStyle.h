#ifndef SVGSTYLE_H
#define SVGSTYLE_H

#include "SvgPen.h"
#include "SvgBrush.h"
#include <QString>

class SvgStyle
{
public:
    SvgStyle();
    SvgStyle(const SvgStyle&) = default;
    SvgStyle& operator=(const SvgStyle& other);
    ~SvgStyle();

    QColor fill() const { return mFill; }
    QColor stroke() const { return mStroke; }
    qreal strokeWidth() const { return mStrokeWidth; }

    SvgPen* pen() const { return mPen; }
    void setPen(SvgPen* pen);

    SvgBrush* brush() const { return mBrush; }
    void setBrush(SvgBrush* brush);

    void applyToPainter(QPainter* painter) const;

    void parseStyleString(const QString& styleStr);
    void parseAttribute(const QString& name, const QString& value);

private:
    SvgPen* mPen;
    SvgBrush* mBrush;
    QColor mFill;         // 填充颜色
    QColor mStroke;       // 描边颜色
    qreal mStrokeWidth;   // 描边宽度

    void copyFrom(const SvgStyle& other);
    void clear();
};

#endif // SVGSTYLE_H
