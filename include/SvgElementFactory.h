#ifndef SVGELEMENTFACTORY_H
#define SVGELEMENTFACTORY_H

#include <QDomElement>
#include <QList>
#include <QPointF>
#include <QPainterPath>

class SvgElement;
class SvgRect;
class SvgCircle;
class SvgText;
class SvgGroup;
class SvgEllipse;   // 新增
class SvgLine;      // 新增
class SvgPolyline;  // 新增
class SvgPolygon;   // 新增
class SvgPath;      // 新增

class SvgElementFactory
{
public:
    // 主创建函数：根据标签名创建对应元素
    static SvgElement* createElement(const QDomElement& domElement);

private:
    // 通用属性解析（样式、变换等）
    static void parseCommonAttributes(SvgElement* element, const QDomElement& domElement);

    // 元素专属创建函数
    static SvgElement* createRectElement(const QDomElement& domElement);
    static SvgElement* createCircleElement(const QDomElement& domElement);
    static SvgElement* createTextElement(const QDomElement& domElement);
    static SvgElement* createGroupElement(const QDomElement& domElement);
    static SvgElement* createEllipseElement(const QDomElement& domElement);  // 新增
    static SvgElement* createLineElement(const QDomElement& domElement);     // 新增
    static SvgElement* createPolylineElement(const QDomElement& domElement); // 新增
    static SvgElement* createPolygonElement(const QDomElement& domElement);  // 新增
    static SvgElement* createPathElement(const QDomElement& domElement);     // 新增

    // 辅助函数：解析points属性
    static QList<QPointF> parsePoints(const QString& pointsStr);
    // 辅助函数：计算点列表的边界框
    static QRectF calculatePointsBoundingBox(const QList<QPointF>& points);
    // 辅助函数：解析路径d属性
    static QPainterPath parsePathData(const QString& d);
};

#endif // SVGELEMENTFACTORY_H
