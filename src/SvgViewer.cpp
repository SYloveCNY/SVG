#include "SvgViewer.h"
#include <QPainter>
#include <QResizeEvent>
#include <QMessageBox>
#include <QDebug>

SvgViewer::SvgViewer(const QString& svgFilePath, QWidget *parent)
    : QWidget(parent), mSvgDocument(std::make_unique<SvgDocument>())
{
    // 设置窗口标题和初始大小
    setWindowTitle("SVG Viewer");
    setMinimumSize(800, 600);

    // 尝试加载SVG文件
    if (!svgFilePath.isEmpty()) {
        if (!loadSvgFile(svgFilePath)) {
            QMessageBox::critical(this, "Error", "Failed to load SVG file: " + svgFilePath);
        }
    }
}

bool SvgViewer::loadSvgFile(const QString& filePath)
{
    bool loaded = mSvgDocument->load(filePath);
    qDebug() << "SVG加载结果：" << loaded;  // 需包含#include <QDebug>
    qDebug() << "解析到的元素数量：" << mSvgDocument->elements().size();

    if (mSvgDocument->load(filePath)) {
        mCurrentFilePath = filePath;
        update();  // 触发重绘
        return true;
    }
    return false;
}

void SvgViewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    qDebug() << "进入paintEvent，窗口尺寸：" << size();  // 确认事件被触发

    QPainter painter(this);  // 绑定到当前窗口
    if (!painter.isActive()) {
        qDebug() << "QPainter初始化失败！";
        return;
    }

    painter.fillRect(rect(), Qt::lightGray);  // 浅灰色背景
    qDebug() << "已绘制背景";

    // 检查mSvgDocument状态
    if (!mSvgDocument) {
        qDebug() << "mSvgDocument为空，不调用render";
        return;
    }
    qDebug() << "mSvgDocument->isValid()：" << mSvgDocument->isValid();
    qDebug() << "mSvgDocument元素数量：" << mSvgDocument->elements().size();

    // 调用渲染方法
    if (mSvgDocument->isValid()) {
        QRectF viewportRect = rect();  // 窗口可视区域
        qDebug() << "准备调用mRenderer.render，viewport：" << viewportRect;
        mRenderer.render(mSvgDocument.get(), &painter, viewportRect);
        qDebug() << "mRenderer.render调用完成";
    } else {
        qDebug() << "mSvgDocument无效，不调用render";
    }

    // if (!mSvgDocument || !mSvgDocument->isValid()) {
    //     mRenderer.render(mSvgDocument.get(), &painter, QRectF(rect()));
    // }

    // // 创建 painter 并开始绘制
    // QPainter painter(this);
    // painter.setRenderHint(QPainter::Antialiasing, true);  // 抗锯齿
    // painter.setRenderHint(QPainter::TextAntialiasing, true);

    // // 获取当前窗口大小作为绘制区域
    // QRectF viewport(0, 0, width(), height());

    // // 渲染SVG到窗口
    // mRenderer.render(mSvgDocument.get(), &painter, viewport);
}

void SvgViewer::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    // 窗口大小变化时触发重绘
    update();
}
