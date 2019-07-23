#include "MainWindow.h"
#include <processors/LogDebugOutput.h>
#include <processors/LogFileSaver.h>
#include <QtWidgets/QApplication>
#include <type_traits>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    auto debug = new log4qt::LogDebugOutput(&app);
    debug->start();

    auto normal = new log4qt::LogFileNormalSaver(&app);
    normal->setDir(QStringLiteral("./normal"));
    normal->start();

    auto mmap = new log4qt::LogFileMmapSaver(&app);
    mmap->setDir(QStringLiteral("./mmap"));
    mmap->start();

    MainWindow w;
    w.show();

    qDebug() << "Hello World!";
    qInfo() << "Hello World!";
    qWarning() << "Hello World!";
    qCritical() << "Hello World!";
//    qFatal("Hello World!");

    return app.exec();
}
