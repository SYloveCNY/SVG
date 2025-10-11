#ifndef SVGVIEWER_H
#define SVGVIEWER_H

#include <QWidget>
#include <memory>
#include "SvgDocument.h"
#include "SvgRenderer.h"

class SvgViewer : public QWidget
{
    Q_OBJECT

public:
    explicit SvgViewer(const QString& svgFilePath, QWidget *parent = nullptr);
    ~SvgViewer() override = default;

    bool loadSvgFile(const QString& filePath);

protected:
    // 重写绘制事件
    void paintEvent(QPaintEvent *event) override;
    // 重写窗口大小变化事件
    void resizeEvent(QResizeEvent *event) override;

private:
    std::unique_ptr<SvgDocument> mSvgDocument;  // SVG文档
    SvgRenderer mRenderer;                      // SVG渲染器
    QString mCurrentFilePath;                   // 当前加载的SVG文件路径
};

#endif // SVGVIEWER_H
