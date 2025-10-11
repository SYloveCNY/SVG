#ifndef SVGTRANSFORM_H
#define SVGTRANSFORM_H

#include <QTransform>
#include <QString>

class SvgTransform
{
public:
    SvgTransform();
    SvgTransform(const QTransform& transform);
    ~SvgTransform();

    // 判断是否为单位矩阵（无变换）
    bool isIdentity() const {
        return mTransform.isIdentity();
    }

    void translate(qreal dx, qreal dy);
    void scale(qreal sx, qreal sy);
    void rotate(qreal angle);
    void rotate(qreal angle, qreal cx, qreal cy);
    void skewX(qreal angle);
    void skewY(qreal angle);

    void parseTransformString(const QString& transformStr);

    void applyTransform(const QTransform& transform);
    void reset();
    bool parseTransform(const QString& transformStr);

    QTransform toQTransform() const;
    void setTransform(const QTransform& transform);

    SvgTransform combined(const SvgTransform& other) const;
    SvgTransform& operator*=(const SvgTransform& other);

private:
    QTransform mTransform;
};

#endif // SVGTRANSFORM_H
