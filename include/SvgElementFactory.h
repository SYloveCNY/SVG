#ifndef SVGELEMENTFACTORY_H
#define SVGELEMENTFACTORY_H

#include "SvgDocument.h"
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
class SvgDocument;

class SvgElementFactory
{
public:
    struct ParsedValue {
        qreal value;    // 数值部分（如"100px"中的100）
        QString unit;   // 单位部分（如"100px"中的"px"）
        bool valid;     // 解析是否成功（true表示有效）
    };

    // 主创建函数：根据标签名创建对应元素
    static SvgElement* createElement(const QDomElement& domElement, SvgDocument* document);

private:
    // 通用属性解析（样式、变换等）
    static void parseCommonAttributes(SvgElement* element, const QDomElement& domElement);

    // 元素专属创建函数
    static SvgElement* createRectElement(const QDomElement& domElement);
    static SvgElement* createCircleElement(const QDomElement& domElement);
    static SvgElement* createTextElement(const QDomElement& domElement);
    static SvgElement* createGroupElement(const QDomElement& domElement, SvgDocument* document);
    static SvgElement* createEllipseElement(const QDomElement& domElement);  // 新增
    static SvgElement* createLineElement(const QDomElement& domElement);     // 新增
    static SvgElement* createPolylineElement(const QDomElement& domElement, SvgDocument* document); // 新增
    static SvgElement* createPolygonElement(const QDomElement& domElement, SvgDocument* document);  // 新增
    static SvgElement* createPathElement(const QDomElement& domElement);     // 新增

    // 声明所有辅助函数
    static QList<qreal> parseNumbers(const QString& str);
    static QList<QPointF> parsePoints(const QString& pointsStr, const QRectF& viewBox);
    static QRectF calculatePointsBoundingBox(const QList<QPointF>& points);
    static QRectF normalizeBbox(const QRectF& bbox);
    static QPainterPath parsePathData(const QString& d);
    static ParsedValue parseValueWithUnit(const QString& str);
    static qreal convertToPx(const ParsedValue& parsed, const QRectF& viewBox);
    static qreal parseDoubleAttr(
        const QDomElement& elem,
        const QString& attrName,
        const QRectF& viewBox = QRectF(),  // 默认空viewBox
        qreal defaultValue = 0             // 默认值0
        );
    static qreal parseDoubleAttrFromString(const QString& str, const QRectF& viewBox);
    // 解析DOM元素的尺寸属性（如x、y、width等），返回数值（默认单位为px）
    static qreal parseDimensionAttr(const QDomElement& elem, const QString& attrName, qreal defaultValue);

    // 解析尺寸字符串（如"250"、"20px"、"1.5em"），提取数值部分（忽略单位）
    static qreal parseDimension(const QString& dimStr, qreal defaultValue);
};

#endif // SVGELEMENTFACTORY_H
