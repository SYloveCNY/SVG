#ifndef SVGELEMENTFACTORY_H
#define SVGELEMENTFACTORY_H

#include <QDomElement>
#include "SvgElement.h"

// 元素类工厂（负责创建SvgElement所有子类实例）
class SvgElementFactory
{
public:
    // 禁止实例化（静态工厂模式）
    SvgElementFactory() = delete;
    ~SvgElementFactory() = delete;

    // 根据DOM标签创建对应元素（核心工厂方法）
    static SvgElement* createElement(const QDomElement& domElement);

private:
    // 辅助方法：解析通用属性（id、transform、style）
    static void parseCommonAttributes(SvgElement* element, const QDomElement& domElement);

    // 具体元素创建方法（工厂内部实现）
    static SvgElement* createRectElement(const QDomElement& domElement);
    static SvgElement* createCircleElement(const QDomElement& domElement);
    static SvgElement* createTextElement(const QDomElement& domElement);
    static SvgElement* createGroupElement(const QDomElement& domElement);
};

#endif // SVGELEMENTFACTORY_H
