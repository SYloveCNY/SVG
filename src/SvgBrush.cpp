#include "SvgBrush.h"
#include <QLinearGradient>
#include <QRadialGradient>
#include <algorithm> // 用于 std::sort

// SvgGradientStop 类的方法实现
qreal SvgGradientStop::getOffset() const {
    return offset;
}

void SvgGradientStop::setOffset(qreal offset) {
    this->offset = offset;
}

QColor SvgGradientStop::getColor() const {
    return color;
}

void SvgGradientStop::setColor(const QColor& color) {
    this->color = color;
}

void SvgGradientStop::setColor(const QString& colorStr) {
    this->color = QColor(colorStr);
}
