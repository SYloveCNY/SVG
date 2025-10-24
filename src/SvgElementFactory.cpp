#include "SvgElementFactory.h"
#include "SvgRect.h"
#include "SvgCircle.h"
#include "SvgText.h"
#include "SvgGroup.h"
#include "SvgEllipse.h"   // 新增：椭圆元素
#include "SvgLine.h"      // 新增：直线元素
#include "SvgPolyline.h"  // 新增：折线元素
#include "SvgPolygon.h"   // 新增：多边形元素
#include "SvgPath.h"      // 新增：路径元素
#include "SvgTransform.h"
#include "SvgStyle.h"
#include <QDomElement>
#include <QDebug>
#include <QRegularExpression>
#include <QFont>
#include <QFontMetricsF>

// 核心：扩展标签识别
SvgElement* SvgElementFactory::createElement(const QDomElement& domElement, SvgDocument* document) {
    const QString tagName = domElement.tagName().toLower();

    if (tagName == "svg") {
        auto* rootGroup = new SvgGroup();
        parseCommonAttributes(rootGroup, domElement);

        // 解析viewBox（此时document参数有效）
        if (domElement.hasAttribute("viewBox")) {
            QString viewBoxStr = domElement.attribute("viewBox");
            QList<qreal> viewBoxVals = parseNumbers(viewBoxStr);
            if (viewBoxVals.size() == 4 && document) {  // 这里document已通过参数传入
                document->setViewBox(QRectF(
                    viewBoxVals[0], viewBoxVals[1],
                    viewBoxVals[2], viewBoxVals[3]
                    ));
            }
        }

        // 递归解析子元素（必须传递document参数）
        QDomNode childNode = domElement.firstChild();
        while (!childNode.isNull()) {
            if (childNode.isElement()) {
                // 调用带document参数的重载版本
                SvgElement* childElem = createElement(childNode.toElement(), document);
                if (childElem) rootGroup->addChild(childElem);
            }
            childNode = childNode.nextSibling();
        }
        return rootGroup;
    } else if (tagName == "rect") {
        return createRectElement(domElement);
    } else if (tagName == "circle") {
        return createCircleElement(domElement);
    } else if (tagName == "ellipse") {
        return createEllipseElement(domElement);
    } else if (tagName == "line") {
        return createLineElement(domElement);
    } else if (tagName == "polyline") {
        return createPolylineElement(domElement, document);
    } else if (tagName == "polygon") {
        return createPolygonElement(domElement, document);
    } else if (tagName == "path") {
        return createPathElement(domElement);
    } else if (tagName == "text") {
        qDebug() << "解析文本元素，内容：" << domElement.text();
        return createTextElement(domElement);
    } else if (tagName == "g") {
        return createGroupElement(domElement, document);
    }

    qDebug() << "未支持的元素：" << tagName;
    return nullptr;
}

// 通用属性解析（无需修改，已支持所有元素的通用样式/变换）
void SvgElementFactory::parseCommonAttributes(SvgElement* element, const QDomElement& domElement)
{
    if (!element) return;

    // 1. 解析ID
    if (domElement.hasAttribute("id")) {
        element->setId(domElement.attribute("id"));
    }

    // 2. 解析Transform（修正变量名：domElem→domElement，elem→element）
    if (domElement.hasAttribute("transform")) {  // 修正：domElem→domElement
        QString transformStr = domElement.attribute("transform");  // 修正：domElem→domElement
        SvgTransform transform;
        transform.parse(transformStr);
        element->setTransform(transform);  // 修正：elem→element
        qDebug() << "解析元素transform：" << transformStr << "（元素类型：" << domElement.tagName() << "）";  // 修正：domElem→domElement
    }
    // 3. 解析样式（复用现有逻辑，支持inline style和单独属性）
    SvgStyle style;
    if (domElement.hasAttribute("style")) {
        style.parseStyleString(domElement.attribute("style"));
    }
    // 解析单独的样式属性（fill/stroke等）
    if (domElement.hasAttribute("fill")) {
        QString fillValue = domElement.attribute("fill");
        style.parseAttribute("fill", fillValue);
        qDebug() << "解析fill：" << fillValue;
    }
    if (domElement.hasAttribute("stroke")) {
        QString strokeValue = domElement.attribute("stroke");
        style.parseAttribute("stroke", strokeValue);
        qDebug() << "解析stroke：" << strokeValue;
    }
    if (domElement.hasAttribute("stroke-width")) {
        QString strokeWidthValue = domElement.attribute("stroke-width");
        style.parseAttribute("stroke-width", strokeWidthValue);
        qDebug() << "解析stroke-width：" << strokeWidthValue;
    }

    element->setStyle(style);
    qDebug() << "设置元素样式完成，填充：" << style.fill().name()
             << "描边：" << style.stroke().name();
}

// 以下为原有元素的创建函数（保持不变）
SvgElement* SvgElementFactory::createRectElement(const QDomElement& domElement)
{
    auto* rect = new SvgRect();
    parseCommonAttributes(rect, domElement);

    // 直接解析矩形属性（替换attrs）
    qreal x = parseDoubleAttr(domElement, "x");
    qreal y = parseDoubleAttr(domElement, "y");
    qreal width = parseDoubleAttr(domElement, "width");
    qreal height = parseDoubleAttr(domElement, "height");
    qreal rx = parseDoubleAttr(domElement, "rx", QRectF(), 0);  // 默认为0（直角）
    qreal ry = parseDoubleAttr(domElement, "ry", QRectF(), 0);

    // 设置矩形属性
    rect->setX(x);
    rect->setY(y);
    rect->setWidth(width);
    rect->setHeight(height);
    rect->setRx(rx);
    rect->setRy(ry);

    // 边界框计算
    rect->setBoundingBox(normalizeBbox(QRectF(x, y, width, height)));
    return rect;
}

SvgElement* SvgElementFactory::createCircleElement(const QDomElement& domElement)
{
    auto* circle = new SvgCircle();
    parseCommonAttributes(circle, domElement);

    // 直接解析圆形属性（替换attrs）
    qreal cx = parseDoubleAttr(domElement, "cx");
    qreal cy = parseDoubleAttr(domElement, "cy");
    qreal r = parseDoubleAttr(domElement, "r");

    // 设置圆形属性
    circle->setCenter(QPointF(cx, cy));
    circle->setRadius(r);

    // 边界框计算
    circle->setBoundingBox(normalizeBbox(QRectF(
        cx - r,
        cy - r,
        r * 2,
        r * 2
        )));
    return circle;
}

SvgElement* SvgElementFactory::createTextElement(const QDomElement& domElement)
{
    auto* text = new SvgText();
    // 1. 解析通用属性（确保包含text-anchor等文本特有属性）
    parseCommonAttributes(text, domElement);

    // 2. 解析文本位置（支持带单位的属性值，如"250px"）
    qreal x = parseDimensionAttr(domElement, "x", 0);  // 替换为支持单位的解析函数
    qreal y = parseDimensionAttr(domElement, "y", 0);
    text->setPosition(QPointF(x, y));

    // 3. 解析文本内容（保留原始内容，避免trim误删有效空格）
    text->setText(domElement.text());  // 仅在确认需要时trim（如用户明确要求去空格）

    // 4. 解析字体属性（完善单位处理和容错）
    SvgStyle style = text->style();  // 获取通用属性解析后的基础样式
    // 解析font-family（支持多字体备选，如" Arial, sans-serif"）
    if (domElement.hasAttribute("font-family")) {
        QString family = domElement.attribute("font-family").trimmed();
        // 移除可能的引号（SVG中font-family可能带引号，如font-family="'Arial'"）
        family.remove(QRegularExpression("^['\"]|['\"]$"));
        style.parseAttribute("font-family", family);
    }
    // 解析font-size（处理单位，如"20px" -> 20）
    if (domElement.hasAttribute("font-size")) {
        QString sizeStr = domElement.attribute("font-size");
        qreal fontSize = parseDimension(sizeStr, 12);  // 自定义函数：提取数值（默认12）
        style.parseAttribute("font-size", QString::number(fontSize));
    }
    // 解析text-anchor（确保覆盖通用属性未处理的情况）
    if (domElement.hasAttribute("text-anchor")) {
        style.parseAttribute("text-anchor", domElement.attribute("text-anchor"));
    }
    text->setStyle(style);  // 更新样式

    // 5. 计算文本边界框（更精确的基线对齐）
    QFont font(style.fontFamily().isEmpty() ? "Arial" : style.fontFamily(),
               style.fontSize() <= 0 ? 12 : style.fontSize());
    QFontMetricsF fm(font);
    // 文本逻辑边界框：x,y为基线位置，需调整到顶部起点
    QRectF textBbox = fm.boundingRect(QRectF(), Qt::AlignLeft, text->text());
    textBbox.translate(x - textBbox.left(),  // 对齐x坐标（左起点）
                       y - fm.ascent());      // 对齐y坐标（基线 -> 顶部）
    text->setBoundingBox(normalizeBbox(textBbox));

    // 调试日志：验证解析结果
    qDebug() << "创建文本元素：内容='" << text->text()
             << "'，位置=(" << x << "," << y << ")"
             << "，字体=" << style.fontFamily()
             << "，大小=" << style.fontSize()
             << "，text-anchor=" << style.textAnchor();

    return text;
}

SvgElement* SvgElementFactory::createGroupElement(const QDomElement& domElement, SvgDocument* document)
{
    auto* group = new SvgGroup();
    parseCommonAttributes(group, domElement);

    QDomNode childNode = domElement.firstChild();
    while (!childNode.isNull()) {
        if (childNode.isElement()) {
            SvgElement* child = createElement(childNode.toElement(), document);
            if (child) group->addChild(child);
        }
        childNode = childNode.nextSibling();
    }

    // 计算组的边界框（合并所有子元素的边界框）
    QRectF groupBbox;
    foreach (const SvgElement* child, group->children()) {
        if (groupBbox.isEmpty()) {
            groupBbox = child->boundingBox();
        } else {
            groupBbox = groupBbox.united(child->boundingBox());
        }
    }
    group->setBoundingBox(groupBbox);
    return group;
}

// 椭圆元素创建与属性解析
SvgElement* SvgElementFactory::createEllipseElement(const QDomElement& domElement)
{
    auto* ellipse = new SvgEllipse();
    parseCommonAttributes(ellipse, domElement);

    // 直接解析椭圆属性（替换attrs）
    qreal cx = parseDoubleAttr(domElement, "cx");
    qreal cy = parseDoubleAttr(domElement, "cy");
    qreal rx = parseDoubleAttr(domElement, "rx");
    qreal ry = parseDoubleAttr(domElement, "ry");

    // 设置椭圆属性
    ellipse->setCx(cx);
    ellipse->setCy(cy);
    ellipse->setRx(rx);
    ellipse->setRy(ry);

    // 计算边界框
    ellipse->setBoundingBox(normalizeBbox(QRectF(
        cx - rx,
        cy - ry,
        rx * 2,
        ry * 2
        )));
    return ellipse;
}

// 直线元素创建与属性解析
SvgElement* SvgElementFactory::createLineElement(const QDomElement& domElement)
{
    auto* line = new SvgLine();
    parseCommonAttributes(line, domElement);

    // 直接解析x1/y1/x2/y2属性（替换attrs）
    qreal x1 = parseDoubleAttr(domElement, "x1");  // 假设已实现parseDoubleAttr（支持单位）
    qreal y1 = parseDoubleAttr(domElement, "y1");
    qreal x2 = parseDoubleAttr(domElement, "x2");
    qreal y2 = parseDoubleAttr(domElement, "y2");

    // 设置直线属性
    line->setX1(x1);
    line->setY1(y1);
    line->setX2(x2);
    line->setY2(y2);

    // 边界框计算（基于实际解析的坐标）
    qreal minX = qMin(x1, x2);
    qreal minY = qMin(y1, y2);
    line->setBoundingBox(normalizeBbox(QRectF(
        minX, minY,
        qMax(x1, x2) - minX,
        qMax(y1, y2) - minY
        )));
    return line;
}

// 折线元素创建与属性解析
SvgElement* SvgElementFactory::createPolylineElement(const QDomElement& domElement, SvgDocument* document)
{
    auto* polyline = new SvgPolyline();
    parseCommonAttributes(polyline, domElement);

    // 解析折线特有属性：points(坐标列表，如"0,0 100,50 200,0")
    if (domElement.hasAttribute("points")) {
        QRectF viewBox = document ? document->viewBox() : QRectF();
        QList<QPointF> points = parsePoints(domElement.attribute("points"), viewBox);
        polyline->setPoints(points);
    }

    // 设置边界框（包含所有点的最小矩形）
    polyline->setBoundingBox(calculatePointsBoundingBox(polyline->points()));
    qDebug() << "创建折线元素：" << polyline->points().size() << "个点";
    return polyline;
}

// 多边形元素创建与属性解析（与折线类似，但自动闭合）
SvgElement* SvgElementFactory::createPolygonElement(const QDomElement& domElement, SvgDocument* document)
{
    auto* polygon = new SvgPolygon();
    parseCommonAttributes(polygon, domElement);

    // 解析多边形特有属性：points(坐标列表)
    if (domElement.hasAttribute("points")) {
        QRectF viewBox = document ? document->viewBox() : QRectF();
        QList<QPointF> points = parsePoints(domElement.attribute("points"), viewBox);
        polygon->setPoints(points);
    }

    // 设置边界框
    polygon->setBoundingBox(calculatePointsBoundingBox(polygon->points()));
    qDebug() << "创建多边形元素：" << polygon->points().size() << "个点";
    return polygon;
}

// 路径元素创建与属性解析（最复杂，需解析d属性）
SvgElement* SvgElementFactory::createPathElement(const QDomElement& domElement)
{
    auto* path = new SvgPath();
    parseCommonAttributes(path, domElement);

    // 解析路径特有属性：d(路径命令，如"M10,10 L100,10 Z")
    if (domElement.hasAttribute("d")) {
        QString d = domElement.attribute("d");
        QPainterPath painterPath = parsePathData(d);
        path->setPath(painterPath);
    }

    // 设置边界框（路径的外接矩形）
    path->setBoundingBox(path->path().boundingRect());
    qDebug() << "创建路径元素：" << path->path().elementCount() << "个路径段";
    return path;
}

QList<qreal> SvgElementFactory::parseNumbers(const QString& str) {
    QList<qreal> numbers;
    QStringList parts = str.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    foreach (const QString& part, parts) {
        bool ok;
        qreal num = part.toDouble(&ok);
        if (ok) {
            numbers.append(num);
        }
    }
    return numbers;
}

// 辅助函数：解析points属性（将"x1,y1 x2,y2"转换为QPointF列表）
QList<QPointF> SvgElementFactory::parsePoints(const QString& pointsStr, const QRectF& viewBox)
{
    QList<QPointF> points;
    if (pointsStr.isEmpty()) return points;

    // 按空格拆分每个点
    QStringList pointList = pointsStr.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    foreach (const QString& pointStr, pointList) {
        // 按逗号拆分x和y
        QStringList coords = pointStr.split(',');
        if (coords.size() != 2) {
            qDebug() << "无效的点格式：" << pointStr;
            continue;
        }

        // 解析x和y（支持单位，如"100px"、"5cm"）
        qreal x = parseDoubleAttrFromString(coords[0].trimmed(), viewBox);
        qreal y = parseDoubleAttrFromString(coords[1].trimmed(), viewBox);

        points.append(QPointF(x, y));
    }
    return points;
}

// 辅助函数：计算点列表的边界框
QRectF SvgElementFactory::calculatePointsBoundingBox(const QList<QPointF>& points)
{
    if (points.isEmpty()) return QRectF();

    qreal minX = points[0].x(), maxX = points[0].x();
    qreal minY = points[0].y(), maxY = points[0].y();

    foreach (const QPointF& p, points) {
        minX = qMin(minX, p.x());
        maxX = qMax(maxX, p.x());
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }
    return QRectF(minX, minY, maxX - minX, maxY - minY);
}

QRectF SvgElementFactory::normalizeBbox(const QRectF& bbox) {
    if (bbox.isEmpty()) {
        return QRectF(0, 0, 0, 0); // 空框返回默认值
    }
    // 确保宽高非负（避免无效边界框）
    qreal x = bbox.x();
    qreal y = bbox.y();
    qreal width = qMax(0.0, bbox.width());
    qreal height = qMax(0.0, bbox.height());
    return QRectF(x, y, width, height);
}

// 辅助函数：解析路径d属性（转换为QPainterPath）
QPainterPath SvgElementFactory::parsePathData(const QString& d)
{
    QPainterPath path;
    if (d.isEmpty()) return path;

    QRegularExpression cmdRegex("[MLCZAQHVLmlczaqhvl]"); // 路径命令
    // 支持整数、小数、科学计数法（含负数）
    QRegularExpression numRegex("-?(?:\\d+\\.?\\d*|\\.\\d+)(?:[eE][-+]?\\d+)?");

    int pos = 0;
    QPointF currentPos;
    bool isRelative = false;

    while (pos < d.size()) {
        // 跳过空白字符（提前处理空格，避免干扰命令匹配）
        while (pos < d.size() && d[pos].isSpace()) {
            pos++;
        }

        // 匹配路径命令
        QRegularExpressionMatch cmdMatch = cmdRegex.match(d, pos);
        if (cmdMatch.hasMatch() && cmdMatch.capturedStart() == pos) {
            QChar cmd = cmdMatch.captured(0).at(0);
            isRelative = cmd.isLower();
            char cmdUpper = cmd.toUpper().toLatin1();
            pos += cmdMatch.capturedLength();

            // 提取当前命令的所有参数
            QList<qreal> params;
            while (pos < d.size()) {
                // 跳过参数间的分隔符（空格或逗号）
                while (pos < d.size() && (d[pos].isSpace() || d[pos] == ',')) {
                    pos++;
                }
                if (pos >= d.size()) break;

                // 提取数字参数
                QRegularExpressionMatch numMatch = numRegex.match(d, pos);
                if (!numMatch.hasMatch() || numMatch.capturedStart() != pos) {
                    break; // 非数字，退出参数提取
                }

                params.append(numMatch.captured(0).toDouble());
                pos += numMatch.capturedLength();
            }

            // 处理不同命令（逻辑不变）
            switch (cmdUpper) {
            case 'Q': {
                // Q命令需要4个参数（x1,y1,x,y），或4的倍数（多段曲线）
                if (params.size() % 4 != 0) {
                    qDebug() << "Q命令参数数量错误（应为4的倍数），实际数量：" << params.size();
                    break;
                }
                // 处理每一段曲线（支持多段Q命令，如Q x1,y1 x,y x1,y1 x,y）
                for (int i = 0; i < params.size(); i += 4) {
                    qreal x1 = params[i];
                    qreal y1 = params[i+1];
                    qreal x = params[i+2];
                    qreal y = params[i+3];

                    if (isRelative) {
                        x1 += currentPos.x();
                        y1 += currentPos.y();
                        x += currentPos.x();
                        y += currentPos.y();
                    }

                    path.quadTo(x1, y1, x, y);
                    currentPos = QPointF(x, y);
                    qDebug() << "解析Q命令：x1=" << x1 << "y1=" << y1 << "x=" << x << "y=" << y;
                }
                break;
            }
            case 'C': { // 三次贝塞尔曲线 (x1,y1,x2,y2,x,y)，支持多组参数
                int paramCount = params.size();
                for (int i = 0; i + 5 < paramCount; i += 6) { // 每组6个参数
                    qreal x1 = params[i], y1 = params[i+1];
                    qreal x2 = params[i+2], y2 = params[i+3];
                    qreal x = params[i+4], y = params[i+5];
                    if (isRelative) {
                        x1 += currentPos.x(); y1 += currentPos.y();
                        x2 += currentPos.x(); y2 += currentPos.y();
                        x += currentPos.x(); y += currentPos.y();
                    }
                    path.cubicTo(x1, y1, x2, y2, x, y);
                    currentPos = QPointF(x, y);
                }
                if (paramCount % 6 != 0) {
                    qWarning() << "C命令参数数量不完整（每组6个），实际数量：" << paramCount;
                }
                break;
            }
            case 'M': { // 移动命令，支持多组参数（后续参数视为L命令）
                if (params.size() >= 2) {
                    // 处理第一个点（移动）
                    qreal x = params[0], y = params[1];
                    if (isRelative) { x += currentPos.x(); y += currentPos.y(); }
                    path.moveTo(x, y);
                    currentPos = QPointF(x, y);
                    qDebug() << "解析M命令：移动到" << x << "," << y;

                    // 处理剩余参数（视为连续的L命令）
                    int paramCount = params.size();
                    for (int i = 2; i + 1 < paramCount; i += 2) {
                        qreal lx = params[i], ly = params[i+1];
                        if (isRelative) { lx += currentPos.x(); ly += currentPos.y(); }
                        path.lineTo(lx, ly);
                        currentPos = QPointF(lx, ly);
                        qDebug() << "M命令后续点视为L：" << lx << "," << ly;
                    }
                }
                break;
            }
            case 'L': { // 直线命令，支持多组参数
                int paramCount = params.size();
                for (int i = 0; i + 1 < paramCount; i += 2) { // 每组2个参数
                    qreal x = params[i], y = params[i+1];
                    if (isRelative) { x += currentPos.x(); y += currentPos.y(); }
                    path.lineTo(x, y);
                    currentPos = QPointF(x, y);
                }
                if (paramCount % 2 != 0) {
                    qWarning() << "L命令参数数量不完整（每组2个），实际数量：" << paramCount;
                }
                break;
            }
            case 'H': { // 水平直线（仅x坐标，y沿用currentPos.y()）
                int paramCount = params.size();
                for (int i = 0; i < paramCount; i++) { // 每个参数都是一个x
                    qreal x = params[i];
                    qreal y = currentPos.y(); // 保持当前y不变
                    if (isRelative) { x += currentPos.x(); }
                    path.lineTo(x, y);
                    currentPos = QPointF(x, y);
                    qDebug() << "解析H命令：x=" << x << "y=" << y;
                }
                break;
            }
            case 'V': { // 垂直直线（仅y坐标，x沿用currentPos.x()）
                int paramCount = params.size();
                for (int i = 0; i < paramCount; i++) { // 每个参数都是一个y
                    qreal y = params[i];
                    qreal x = currentPos.x(); // 保持当前x不变
                    if (isRelative) { y += currentPos.y(); }
                    path.lineTo(x, y);
                    currentPos = QPointF(x, y);
                    qDebug() << "解析V命令：x=" << x << "y=" << y;
                }
                break;
            }
            case 'Z': { // 闭合路径
                path.closeSubpath();
                currentPos = path.currentPosition();
                qDebug() << "解析Z命令：闭合路径，当前位置重置为" << currentPos;
                break;
            }
            case 'A': {
                if (params.size() % 7 != 0) {
                    qWarning() << "A命令参数数量错误（应为7的倍数），实际数量：" << params.size();
                    break;
                }

                for (int i = 0; i < params.size(); i += 7) {
                    qreal rx = params[i];
                    qreal ry = params[i+1];
                    qreal xRot = params[i+2];
                    bool largeArc = params[i+3] != 0;
                    bool sweep = params[i+4] != 0;  // 1=顺时针，0=逆时针
                    qreal x = params[i+5];
                    qreal y = params[i+6];

                    if (isRelative) {
                        x += currentPos.x();
                        y += currentPos.y();
                    }

                    if (rx <= 0 || ry <= 0) {
                        path.lineTo(x, y);
                        currentPos = QPointF(x, y);
                        continue;
                    }

                    QPointF start = currentPos;
                    QPointF end(x, y);

                    // ---------------------------
                    // 关键修正1：精确计算椭圆中心（参考SVG规范）
                    // ---------------------------
                    // 1. 将起点和终点转换到以原点为中心的坐标系（考虑旋转）
                    qreal rad = qDegreesToRadians(xRot);
                    qreal cosRot = qCos(rad);
                    qreal sinRot = qSin(rad);

                    // 平移起点和终点到原点（减去中点）
                    qreal dx = (start.x() - end.x()) / 2.0;
                    qreal dy = (start.y() - end.y()) / 2.0;
                    // 应用旋转逆变换（消除x轴旋转的影响）
                    qreal x1 = cosRot * dx + sinRot * dy;
                    qreal y1 = -sinRot * dx + cosRot * dy;

                    // 2. 计算椭圆中心（简化版，完整逻辑需解二次方程）
                    qreal rxSq = rx * rx;
                    qreal rySq = ry * ry;
                    qreal x1Sq = x1 * x1;
                    qreal y1Sq = y1 * y1;

                    // 修正半径（避免数值问题）
                    qreal scale = x1Sq / rxSq + y1Sq / rySq;
                    if (scale > 1) {
                        rx *= qSqrt(scale);
                        ry *= qSqrt(scale);
                        rxSq = rx * rx;
                        rySq = ry * ry;
                    }

                    // 计算中心在旋转坐标系中的位置
                    qreal cX = qSqrt(rxSq * rySq - rxSq * y1Sq - rySq * x1Sq) / qSqrt(rxSq * y1Sq + rySq * x1Sq);
                    qreal cY = 0;
                    if (largeArc == sweep) {  // 调整中心方向
                        cX = -cX;
                    }
                    cX = cX * rx * y1 / ry;
                    cY = -cY * ry * x1 / rx;

                    // 3. 将中心转换回原始坐标系
                    QPointF center;
                    center.setX((start.x() + end.x()) / 2.0 + cosRot * cX - sinRot * cY);
                    center.setY((start.y() + end.y()) / 2.0 + sinRot * cX + cosRot * cY);

                    // ---------------------------
                    // 关键修正2：适配Y轴向下的角度计算
                    // ---------------------------
                    // 计算起点和终点相对于中心的向量（Y轴向下，角度需反向）
                    qreal startVecX = start.x() - center.x();
                    qreal startVecY = -(start.y() - center.y());  // Y轴反向（关键）
                    qreal endVecX = end.x() - center.x();
                    qreal endVecY = -(end.y() - center.y());      // Y轴反向

                    // 计算起始角度和终点角度（弧度转度）
                    qreal startAngle = qRadiansToDegrees(qAtan2(startVecY, startVecX));
                    qreal endAngle = qRadiansToDegrees(qAtan2(endVecY, endVecX));

                    // ---------------------------
                    // 关键修正3：正确映射sweep-flag到角度方向
                    // ---------------------------
                    qreal spanAngle = endAngle - startAngle;
                    // 处理大弧标志
                    if (largeArc) {
                        if (spanAngle > 0) spanAngle -= 360;
                        else spanAngle += 360;
                    }
                    // 处理扫描方向（sweep=1→顺时针→spanAngle为负）
                    if (sweep) {
                        spanAngle = -qAbs(spanAngle);  // 强制顺时针（负值）
                    } else {
                        spanAngle = qAbs(spanAngle);   // 强制逆时针（正值）
                    }

                    // 绘制椭圆弧
                    if (path.currentPosition() != start) {
                        path.moveTo(start);
                    }
                    QRectF ellipseRect(center.x() - rx, center.y() - ry, 2*rx, 2*ry);
                    path.arcTo(ellipseRect, startAngle, spanAngle);

                    currentPos = end;
                    qDebug() << "解析A命令：开口方向已修正，终点=" << x << "," << y;
                }
                break;
            }

            default:
                qDebug() << "未支持的路径命令：" << cmdUpper;
            }
        } else {
            pos++; // 跳过非命令字符（空格、逗号等）
        }
    }
    qDebug() << "解析path成功，命令数量：" << path.elementCount();
    return path;
}

SvgElementFactory::ParsedValue SvgElementFactory::parseValueWithUnit(const QString& str) {
    ParsedValue res = {0, "", false};
    if (str.isEmpty()) return res;

    // 分离数值和单位（跳过正负号、小数点）
    int i = 0;
    if (str[i] == '+' || str[i] == '-') i++;
    while (i < str.size() && (str[i].isDigit() || str[i] == '.')) i++;

    // 提取数值
    QString numStr = str.left(i).trimmed();
    bool ok;
    res.value = numStr.toDouble(&ok);
    if (!ok) {
        qDebug() << "数值解析失败：" << str;
        return res;
    }

    // 提取单位（默认px）
    res.unit = str.mid(i).trimmed().toLower();
    if (res.unit.isEmpty()) res.unit = "px";
    res.valid = true;
    return res;
}

qreal SvgElementFactory::convertToPx(const ParsedValue& parsed, const QRectF& viewBox) {
    if (!parsed.valid) return 0;

    const qreal dpi = 96.0; // 标准屏幕DPI
    qreal px = parsed.value;
    if (parsed.unit == "px") {
        px = parsed.value;
    } else if (parsed.unit == "cm") {
        px = parsed.value * dpi / 2.54; // 1cm = 96/2.54 px
    } else if (parsed.unit == "mm") {
        px = parsed.value * dpi / 25.4;
    } else if (parsed.unit == "%") {
        px = parsed.value * viewBox.width() / 100.0; // 百分比相对viewBox宽度
    } else if (parsed.unit == "em") {
        px = parsed.value * 16.0; // 默认字体大小16px
    }
    return px;
}

qreal SvgElementFactory::parseDoubleAttr(const QDomElement& elem, const QString& attrName,
                                         const QRectF& viewBox, qreal defaultValue) {
    if (!elem.hasAttribute(attrName)) return defaultValue;

    QString attrVal = elem.attribute(attrName).trimmed();
    ParsedValue parsed = parseValueWithUnit(attrVal);
    if (!parsed.valid) {
        qDebug() << "[警告] 解析属性失败：" << attrName << "=" << attrVal;
        return defaultValue;
    }

    qreal pxVal = convertToPx(parsed, viewBox);
    qDebug() << "解析属性：" << attrName << "=" << attrVal << "→" << pxVal << "px";
    return pxVal;
}

// 辅助函数：从字符串解析带单位的数值（如"50px"）
qreal SvgElementFactory::parseDoubleAttrFromString(const QString& str, const QRectF& viewBox) {
    ParsedValue parsed = parseValueWithUnit(str);
    return parsed.valid ? convertToPx(parsed, viewBox) : 0;
}

// 解析DOM元素的属性值（如elem.attribute("x")）
qreal SvgElementFactory::parseDimensionAttr(const QDomElement& elem, const QString& attrName, qreal defaultValue) {
    if (!elem.hasAttribute(attrName)) {
        return defaultValue; // 属性不存在时返回默认值
    }
    QString attrValue = elem.attribute(attrName).trimmed();
    return parseDimension(attrValue, defaultValue);
}

// 解析尺寸字符串（提取数值，忽略单位）
qreal SvgElementFactory::parseDimension(const QString& dimStr, qreal defaultValue) {
    if (dimStr.isEmpty()) {
        return defaultValue;
    }
    // 正则表达式：匹配开头的数字（支持正负、小数）
    QRegularExpression regex("^[-+]?\\d+(\\.\\d+)?");
    QRegularExpressionMatch match = regex.match(dimStr);
    if (match.hasMatch()) {
        return match.captured(0).toDouble(); // 提取数值部分并转换为double
    } else {
        return defaultValue; // 匹配失败返回默认值
    }
}
