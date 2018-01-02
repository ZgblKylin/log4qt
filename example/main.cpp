#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    a.setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    MainWindow w;
    w.show();

    return a.exec();
}
