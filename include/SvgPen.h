#ifndef SVGPEN_H
#define SVGPEN_H

#include <QPen>
#include <QColor>

class SvgPen
{
public:
    SvgPen();
    SvgPen(const SvgPen& other);
    virtual ~SvgPen();

    // 克隆方法，创建当前对象的副本
    virtual SvgPen* clone() const;

    QPen toQPen() const;

    QColor color() const;
    void setColor(const QColor& color);

    qreal width() const;
    void setWidth(qreal width);

    Qt::PenStyle style() const;
    void setStyle(Qt::PenStyle style);

    Qt::PenCapStyle capStyle() const;
    void setCapStyle(Qt::PenCapStyle capStyle);

    Qt::PenJoinStyle joinStyle() const;
    void setJoinStyle(Qt::PenJoinStyle joinStyle);

    QVector<qreal> dashPattern() const;
    void setDashPattern(const QVector<qreal>& pattern);

    SvgPen& operator=(const SvgPen& other);

private:
    QColor mColor;
    qreal mWidth;
    Qt::PenStyle mStyle;
    Qt::PenCapStyle mCapStyle;
    Qt::PenJoinStyle mJoinStyle;
    QVector<qreal> mDashPattern;
};

#endif // SVGPEN_H
