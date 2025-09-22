#include <QApplication>
#include <QPainter>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include "SvgDocument.h"
#include "SvgRenderer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 创建SVG文档并加载文件
    SvgDocument doc;
    bool loaded = doc.load("test.svg");  // 替换为实际的SVG文件路径
    if (!loaded) {
        qDebug() << "无法加载SVG文件";
        return 1;
    }

    // 创建渲染器
    SvgRenderer renderer;
    QImage image(800, 600, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    renderer.setPainter(&painter);
    renderer.setViewBox(doc.viewBox());

    // 渲染文档
    renderer.render(&doc);
    painter.end();

    // 显示渲染结果
    QMainWindow window;
    QLabel* label = new QLabel;
    label->setPixmap(QPixmap::fromImage(image));
    window.setCentralWidget(label);
    window.resize(800, 600);
    window.show();

    // 保存结果
    image.save("output.png");

    return app.exec();
}
