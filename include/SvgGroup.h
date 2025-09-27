#ifndef SVGGROUP_H
#define SVGGROUP_H

#include "SvgElement.h"
#include <QList>

class SvgGroup : public SvgElement
{
public:
    explicit SvgGroup(const QString& id = "");
    ~SvgGroup() override; // 需手动释放子元素

    void draw(SvgRenderer* renderer) const override;
    QRectF boundingBox() const override;

    // 子元素管理
    void addChild(SvgElement* child);
    void removeChild(SvgElement* child);
    QList<SvgElement*> children() const { return mChildren; }

private:
    QList<SvgElement*> mChildren; // 组内子元素列表
};

#endif // SVGGROUP_H
