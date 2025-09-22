#ifndef SVGBRUSH_H
#define SVGBRUSH_H

#include <QBrush>
#include <QColor>
#include <QPointF>
#include <QList>

class SvgBrush {
public:
    enum BrushType {
        Solid,
        LinearGradient,
        RadialGradient,
        Pattern
    };

    virtual ~SvgBrush() = default;
    BrushType type() const { return mType; }

    virtual QBrush toQBrush() const = 0;
    virtual SvgBrush* clone() const = 0;

protected:
    explicit SvgBrush(BrushType type) : mType(type) {}

private:
    BrushType mType;
};

class SvgGradientStop {
public:
    qreal offset;   // 成员变量
    QColor color;   // 成员变量

    SvgGradientStop(qreal offset = 0.0, const QColor& color = Qt::black)
        : offset(offset), color(color) {}

    qreal getOffset() const;
    void setOffset(qreal offset);
    QColor getColor() const;
    void setColor(const QColor& color);
    void setColor(const QString& colorStr);
};

class SvgSolidBrush : public SvgBrush {
public:
    SvgSolidBrush() : SvgBrush(BrushType::Solid) {}
    explicit SvgSolidBrush(const QColor& color)
        : SvgBrush(BrushType::Solid), mColor(color) {}
    explicit SvgSolidBrush(const QString& colorStr)
        : SvgBrush(BrushType::Solid) {
        mColor.setNamedColor(colorStr);
    }

    void setColor(const QColor& color) { mColor = color; }
    void setColor(const QString& colorStr) { mColor.setNamedColor(colorStr); }
    QColor color() const { return mColor; }

    QBrush toQBrush() const override { return QBrush(mColor); }
    SvgSolidBrush* clone() const override {
        return new SvgSolidBrush(*this);
    }

private:
    QColor mColor;
};

class SvgLinearGradientBrush : public SvgBrush {
public:
    SvgLinearGradientBrush() : SvgBrush(BrushType::LinearGradient) {}
    SvgLinearGradientBrush(const QPointF& start, const QPointF& end)
        : SvgBrush(BrushType::LinearGradient), mStart(start), mEnd(end) {}

    void setStart(const QPointF& start) { mStart = start; }
    QPointF start() const { return mStart; }
    void setEnd(const QPointF& end) { mEnd = end; }
    QPointF end() const { return mEnd; }
    void addStop(const SvgGradientStop& stop) { mStops.append(stop); }
    void addStop(qreal offset, const QColor& color) {
        mStops.append(SvgGradientStop(offset, color));
    }
    QList<SvgGradientStop> stops() const { return mStops; }

    QBrush toQBrush() const override {
        QLinearGradient gradient(mStart, mEnd);
        for (const auto& stop : mStops) {
            gradient.setColorAt(stop.getOffset(), stop.getColor()); // 变量直接访问
        }
        return QBrush(gradient);
    }
    SvgLinearGradientBrush* clone() const override { // 返回自身类型
        return new SvgLinearGradientBrush(*this);
    }

private:
    QPointF mStart;
    QPointF mEnd;
    QList<SvgGradientStop> mStops;
};

class SvgRadialGradientBrush : public SvgBrush {
public:
    // 构造函数（现在是独立类的构造函数，非成员函数）
    SvgRadialGradientBrush()
        : SvgBrush(BrushType::RadialGradient), mRadius(0) {}
    SvgRadialGradientBrush(const QPointF& center, qreal radius, const QPointF& focalPoint)
        : SvgBrush(BrushType::RadialGradient),
        mCenter(center),
        mRadius(radius),
        mFocalPoint(focalPoint) {}

    // 成员函数
    void setCenter(const QPointF& center) { mCenter = center; }
    QPointF center() const { return mCenter; }
    void setRadius(qreal radius) { mRadius = radius; }
    qreal radius() const { return mRadius; }
    void setFocalPoint(const QPointF& focalPoint) { mFocalPoint = focalPoint; }
    QPointF focalPoint() const { return mFocalPoint; }
    void addStop(const SvgGradientStop& stop) { mStops.append(stop); }
    void addStop(qreal offset, const QColor& color) {
        mStops.append(SvgGradientStop(offset, color));
    }
    QList<SvgGradientStop> stops() const { return mStops; }

    // 重写纯虚函数
    QBrush toQBrush() const override {
        QRadialGradient gradient(mCenter, mRadius, mFocalPoint);
        for (const auto& stop : mStops) {
            gradient.setColorAt(stop.getOffset(), stop.getColor());
        }
        return QBrush(gradient);
    }
    SvgRadialGradientBrush* clone() const override {
        return new SvgRadialGradientBrush(*this);
    }

private:
    QPointF mCenter;
    qreal mRadius;
    QPointF mFocalPoint;
    QList<SvgGradientStop> mStops;
};

#endif // SVGBRUSH_H
