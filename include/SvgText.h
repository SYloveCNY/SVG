#ifndef SVGTEXT_H
#define SVGTEXT_H

#include "SvgElement.h"
#include <QPointF>
#include <QString>

class SvgText : public SvgElement
{
public:
    explicit SvgText(const QString& id = "");
    ~SvgText() override = default;

    void draw(SvgRenderer* renderer) const override;
    QRectF boundingBox() const override;

    // 属性访问
    QPointF position() const { return mPosition; }
    void setPosition(const QPointF& pos) { mPosition = pos; }

    QString text() const { return mText; }
    void setText(const QString& text) { mText = text; }
    void setTextAnchor(const QString& anchor) { mTextAnchor = anchor; }

private:
    QPointF mPosition{0.0, 0.0};
    QString mText;
    QString mTextAnchor = "start"; // 默认左对齐
};

#endif // SVGTEXT_H
