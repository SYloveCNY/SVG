#include "SvgTransform.h"
#include <QRegularExpression>
#include <QStringList>
#include <cmath>  // 用于角度转弧度（M_PI）和正切计算（tan()）

//  默认构造：初始化为单位矩阵（无变换）
SvgTransform::SvgTransform()
    : mTransform(QTransform())  // 显式初始化，避免默认构造的不确定性
{
}

// 带初始矩阵的构造
SvgTransform::SvgTransform(const QTransform& transform)
    : mTransform(transform)
{
}

// 析构函数（virtual 对应头文件声明）
SvgTransform::~SvgTransform()
{
}

// -------------------------- 补充的函数实现（与头文件声明对应） --------------------------
void SvgTransform::applyTransform(const QTransform& transform)
{
    // 矩阵乘法：当前矩阵 = 当前矩阵 * 传入矩阵（SVG 变换顺序规则）
    mTransform *= transform;
}

void SvgTransform::reset()
{
    // 重置为单位矩阵（无平移、无缩放、无旋转）
    mTransform.reset();
}

// -------------------------- 修正原有变换方法（核心：skew → shear + 角度转换） --------------------------
void SvgTransform::translate(qreal tx, qreal ty)
{
    mTransform.translate(tx, ty);
}

void SvgTransform::scale(qreal sx, qreal sy)
{
    mTransform.scale(sx, sy);
}

void SvgTransform::rotate(qreal angle, qreal cx, qreal cy)
{
    if (cx != 0 || cy != 0) {
        // 绕任意点旋转：平移到原点 → 旋转 → 平移回原位置
        mTransform.translate(cx, cy);
        mTransform.rotate(angle);  // QTransform::rotate 直接支持角度值
        mTransform.translate(-cx, -cy);
    } else {
        mTransform.rotate(angle);
    }
}

void SvgTransform::skewX(qreal angle)
{
    // 关键修正：
    // 1. SVG skewX 是角度 → 转换为弧度（M_PI 是 π，需包含 <cmath>）
    // 2. QTransform::shear(sh, sv) 接收剪切因子 = tan(角度弧度值)
    qreal shearFactor = tan(angle * M_PI / 180.0);
    mTransform.shear(shearFactor, 0.0);  // 水平剪切：x 轴偏移与 y 成正比
}

void SvgTransform::skewY(qreal angle)
{
    // 垂直剪切：y 轴偏移与 x 成正比
    qreal shearFactor = tan(angle * M_PI / 180.0);
    mTransform.shear(0.0, shearFactor);
}

// 获取当前变换矩阵
QTransform SvgTransform::toQTransform() const
{
    return mTransform;
}

// 解析 SVG transform 字符串（如 "translate(10,20) scale(2) rotate(30,50,50)"）
bool SvgTransform::parseTransform(const QString& transformStr)
{
    reset();  // 先清除现有变换，避免累积错误

    // 正则表达式：匹配 "变换类型(参数)" 格式（如 "translate(10,20)"）
    QRegularExpression re("([a-zA-Z]+)\\s*\\(([^)]+)\\)");
    QRegularExpressionMatchIterator matchIter = re.globalMatch(transformStr);

    while (matchIter.hasNext()) {
        QRegularExpressionMatch match = matchIter.next();
        if (!match.hasMatch()) continue;

        // 提取变换类型（如 "translate"）和参数（如 "10,20"）
        QString func = match.captured(1).toLower();  // 不区分大小写（如 "Translate" → "translate"）
        QString paramStr = match.captured(2);

        // 解析参数：支持逗号/空格分隔（如 "10 20" 或 "10,20"）
        QStringList paramList = paramStr.split(
            QRegularExpression("[,\\s]+"),  // 匹配逗号或任意空格
            Qt::SkipEmptyParts  // 跳过空字符串（如多个空格连续）
            );

        // 转换参数为数值（存储到 QVector<qreal>）
        QVector<qreal> params;
        for (const QString& param : paramList) {
            bool ok = false;
            qreal val = param.toDouble(&ok);
            if (!ok) return false;  // 参数不是合法数字，解析失败
            params.append(val);
        }

        // 根据变换类型应用对应的操作
        if (func == "translate") {
            // translate 参数：1个（x）或 2个（x,y），默认 y=0
            qreal tx = params.size() >= 1 ? params[0] : 0.0;
            qreal ty = params.size() >= 2 ? params[1] : 0.0;
            translate(tx, ty);
        } else if (func == "scale") {
            // scale 参数：1个（x=y）或 2个（x,y），默认 y=x
            qreal sx = params.size() >= 1 ? params[0] : 1.0;
            qreal sy = params.size() >= 2 ? params[1] : sx;
            scale(sx, sy);
        } else if (func == "rotate") {
            // rotate 参数：1个（角度，绕原点）或 3个（角度, cx, cy），默认 cx=cy=0
            if (params.size() < 1) return false;
            qreal angle = params[0];
            qreal cx = params.size() >= 3 ? params[1] : 0.0;
            qreal cy = params.size() >= 3 ? params[2] : 0.0;
            rotate(angle, cx, cy);
        } else if (func == "skewx") {
            // skewX 参数：1个（角度）
            if (params.size() < 1) return false;
            skewX(params[0]);
        } else if (func == "skewy") {
            // skewY 参数：1个（角度）
            if (params.size() < 1) return false;
            skewY(params[0]);
        } else if (func == "matrix") {
            // matrix 参数：6个（a,b,c,d,e,f），对应矩阵 [a b; c d; e f]
            if (params.size() < 6) return false;
            QTransform matrix(
                params[0], params[1], params[2],
                params[3], params[4], params[5]
                );
            applyTransform(matrix);
        }
    }

    return true;  // 所有变换解析成功
}

void SvgTransform::parseTransformString(const QString& transformStr)
{
    // 直接复用已实现的parseTransform逻辑
    parseTransform(transformStr);
}
