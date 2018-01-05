#include <processors/log4qtDisplayWidget.h>
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LogDisplayBuffer* buffer = new LogDisplayBuffer(this);
    log4qtDisplayWidget* widget = new log4qtDisplayWidget(buffer, this);
    ui->verticalLayout->addWidget(widget);
    buffer->start();
    widget->start();

    connect(ui->ok, &QPushButton::clicked,
            this, [=]{
        qtCLog(ui->category->text(), ui->level->value()) << ui->message->text();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
