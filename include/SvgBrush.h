#ifndef SVGBRUSH_H
#define SVGBRUSH_H

#include <QBrush>
#include <QColor>
#include <QList>
#include <QPointF>

// 渐变停止点类
class SvgGradientStop
{
public:
    SvgGradientStop() : offset(0.0) {}
    ~SvgGradientStop() = default;

    qreal getOffset() const;
    void setOffset(qreal offset);

    QColor getColor() const;
    void setColor(const QColor& color);
    void setColor(const QString& colorStr);

private:
    qreal offset;
    QColor color;
};

// 画笔基类（抽象类）
class SvgBrush
{
public:
    enum BrushType {
        Solid,
        LinearGradient,
        RadialGradient
    };

    SvgBrush() = default;
    virtual ~SvgBrush() = default;

    // 克隆方法
    virtual SvgBrush* clone() const = 0;
    // 转换为Qt画笔
    virtual QBrush toQBrush() const = 0;
    // 获取画笔类型
    virtual BrushType type() const = 0;
};

// 纯色画笔（子类）
class SvgSolidBrush : public SvgBrush
{
public:
    SvgSolidBrush() : mColor(Qt::black) {}
    SvgSolidBrush(const QColor& color) : mColor(color) {}
    ~SvgSolidBrush() override = default;

    SvgBrush* clone() const override { return new SvgSolidBrush(mColor); }
    QBrush toQBrush() const override { return QBrush(mColor); }
    BrushType type() const override { return Solid; }

    QColor color() const { return mColor; }
    void setColor(const QColor& color) { mColor = color; }

private:
    QColor mColor;
};

// 线性渐变画笔（子类）
class SvgLinearGradientBrush : public SvgBrush
{
public:
    SvgLinearGradientBrush() : mStart(QPointF(0,0)), mEnd(QPointF(1,1)) {}
    ~SvgLinearGradientBrush() override = default;

    SvgBrush* clone() const override {
        auto* copy = new SvgLinearGradientBrush();
        copy->setStart(mStart);
        copy->setEnd(mEnd);
        copy->setStops(mStops);
        return copy;
    }

    QBrush toQBrush() const override {
        QLinearGradient grad(mStart, mEnd);
        for (const auto& stop : mStops) {
            grad.setColorAt(stop.getOffset(), stop.getColor());
        }
        return QBrush(grad);
    }

    BrushType type() const override { return LinearGradient; }

    QPointF start() const { return mStart; }
    void setStart(const QPointF& start) { mStart = start; }

    QPointF end() const { return mEnd; }
    void setEnd(const QPointF& end) { mEnd = end; }

    QList<SvgGradientStop> stops() const { return mStops; }
    void addStop(const SvgGradientStop& stop) { mStops.append(stop); }
    void setStops(const QList<SvgGradientStop>& stops) { mStops = stops; }

private:
    QPointF mStart;
    QPointF mEnd;
    QList<SvgGradientStop> mStops;
};

#endif // SVGBRUSH_H
