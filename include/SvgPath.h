#ifndef SVG_PATH_H
#define SVG_PATH_H

#include "SvgElement.h"
#include <QPainterPath>

class SvgPath : public SvgElement {
public:
    SvgPath() : SvgElement(TypeShape) {}
    void setPath(const QPainterPath& path) { mPath = path; }
    QPainterPath path() const { return mPath; }
    void draw(SvgRenderer* renderer) const override;  // 重写draw

private:
    QPainterPath mPath;
};

#endif // SVG_PATH_H
