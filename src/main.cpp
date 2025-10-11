#include "SvgViewer.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    QCoreApplication::setApplicationName("SVG Viewer");
    QCoreApplication::setApplicationVersion("1.0");

    // 解析命令行参数
    QCommandLineParser parser;
    parser.setApplicationDescription("SVG Viewer - Display SVG files in a window");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "SVG file to open.");

    parser.process(app);
    const QStringList args = parser.positionalArguments();

    // 确定要打开的SVG文件路径
    QString svgFilePath;
    if (!args.isEmpty()) {
        svgFilePath = args.first();
    }

    // 创建并显示主窗口
    SvgViewer viewer(svgFilePath);
    viewer.show();

    return app.exec();
}
