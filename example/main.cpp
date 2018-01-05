#include "MainWindow.h"
#include <processors/log4qtDebugOutput.h>
#include <processors/log4qtFileSaver.h>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    a.setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    auto debug = new log4qtDebugOutput(&a);
    debug->start();

    auto normal = new log4qtFileNormalSaver(&a);
    normal->setProperty("dir", "./normal");
    normal->start();

    auto mmap = new log4qtFileMmapSaver(&a);
    mmap->setProperty("dir", "./mmap");
    mmap->start();

    MainWindow w;
    w.show();

    qtDebug("Hello World!");
    qtInfomation("Hello World!");
    qtWarning("Hello World!");
    qtCritical("Hello World!");
    qtFatal("Hello World!");

    return a.exec();
}
