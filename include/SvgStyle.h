#ifndef SVGSTYLE_H
#define SVGSTYLE_H

#include "SvgPen.h"
#include "SvgBrush.h"
#include <QString>

class SvgStyle
{
public:
    SvgStyle();
    SvgStyle(const SvgStyle& other);
    SvgStyle& operator=(const SvgStyle& other);
    ~SvgStyle();

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

    void copyFrom(const SvgStyle& other);
    void clear();
};

#endif // SVGSTYLE_H
