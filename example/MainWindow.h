#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <log4qt.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow() override final;

private:
    Ui::MainWindow* ui;
    QMap<QByteArray, QLoggingCategory*> categories;
};

#endif // MAINWINDOW_H
