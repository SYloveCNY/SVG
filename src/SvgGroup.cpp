#include "SvgGroup.h"
#include "SvgRenderer.h"
#include <QPainter>

SvgGroup::SvgGroup(const QString& id)
    : SvgElement(TypeGroup, id)
{}

SvgGroup::~SvgGroup()
{
    // 释放所有子元素
    qDeleteAll(mChildren);
    mChildren.clear();
}

void SvgGroup::draw(SvgRenderer* renderer) const
{
    if (!renderer || !renderer->painter()) return;

    QPainter* painter = renderer->painter();
    painter->save(); // 保存组之前的状态

    // 应用组的变换（子元素会继承此变换）
    const SvgTransform& groupTransform = this->transform();
    if (!groupTransform.toQTransform().isIdentity()) {
        painter->setTransform(groupTransform.toQTransform(), true);
    }

    // 绘制所有子元素
    for (SvgElement* child : mChildren) {
        if (child) {
            child->draw(renderer);
        }
    }

    painter->restore(); // 恢复状态
}

QRectF SvgGroup::boundingBox() const
{
    QRectF groupBbox;
    // 合并所有子元素的边界框
    for (SvgElement* child : mChildren) {
        if (child) {
            QRectF childBbox = child->boundingBox();
            // 应用子元素的变换到边界框
            const SvgTransform& childTransform = child->transform();
            childBbox = childTransform.toQTransform().mapRect(childBbox);

            if (groupBbox.isEmpty()) {
                groupBbox = childBbox;
            } else {
                groupBbox = groupBbox.united(childBbox);
            }
        }
    }
    return groupBbox;
}

void SvgGroup::addChild(SvgElement* child)
{
    if (child && !mChildren.contains(child)) {
        mChildren.append(child);
        // 更新组边界框
        setBoundingBox(boundingBox());
    }
}

void SvgGroup::removeChild(SvgElement* child)
{
    if (child) {
        mChildren.removeAll(child);
        delete child; // 移除时释放子元素
        setBoundingBox(boundingBox());
    }
}
