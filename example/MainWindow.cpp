#include <processors/log4qtDisplayWidget.h>
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(new log4qtDisplayWidget(new LogDisplayBuffer(this), this));

    connect(ui->ok, &QPushButton::clicked,
            this, [=]{
        qtLog(ui->level->value()) << ui->message->text();
    });

    qtDebug("Hello World!");
    qtInfomation("Hello World!");
    qtWarning("Hello World!");
    qtCritical("Hello World!");
    qtFatal("Hello World!");
}

MainWindow::~MainWindow()
{
    delete ui;
}
