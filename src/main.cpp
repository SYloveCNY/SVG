#include <QApplication>
#include <QPainter>
#include <QImage>
#include <QCommandLineParser>
#include "SvgDocument.h"
#include "SvgRenderer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("SVG Renderer");
    app.setApplicationVersion("1.0.0");

    // 命令行参数解析（输入SVG文件，输出PNG）
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("input", "Input SVG file path.");
    parser.addPositionalArgument("output", "Output PNG file path.");
    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 2) {
        parser.showHelp(1);
    }
    const QString inputPath = args[0];
    const QString outputPath = args[1];

    // 1. 加载SVG文档
    SvgDocument doc;
    if (!doc.load(inputPath)) {
        qCritical() << "Failed to load SVG file:" << inputPath;
        return 1;
    }

    // 2. 初始化渲染器
    SvgRenderer renderer;
    QRectF viewBox = doc.viewBox();
    // 若未指定viewBox，默认800x600
    const qreal imgWidth = viewBox.isEmpty() ? 800 : viewBox.width();
    const qreal imgHeight = viewBox.isEmpty() ? 600 : viewBox.height();

    QImage image(imgWidth, imgHeight, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    renderer.setPainter(&painter);
    renderer.setViewBox(viewBox);

    // 3. 渲染SVG
    renderer.render(&doc);

    // 4. 保存结果
    if (!image.save(outputPath)) {
        qCritical() << "Failed to save PNG file:" << outputPath;
        return 1;
    }

    qInfo() << "Successfully rendered SVG to:" << outputPath;
    return 0;
}
