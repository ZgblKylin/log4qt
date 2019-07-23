#include <processors/LogDisplayWidget.h>
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    log4qt::LogDisplayWidget* widget = new log4qt::LogDisplayWidget(this);
    ui->verticalLayout->addWidget(widget);
    widget->start();

    connect(ui->ok, &QPushButton::clicked,
            this, [=]{
        QByteArray category = ui->category->text().toUtf8();
        if (!categories.contains(category))
        {
            auto it = categories.insert(category, nullptr);
            it.value() = new QLoggingCategory(it.key().constData());
        }
        QLoggingCategory& c = *(categories[category]);
        switch (QtMsgType(ui->level->currentIndex()))
        {
        case QtDebugMsg:
            qCDebug(c) << ui->message->text();
            break;
        case QtWarningMsg:
            qCWarning(c) << ui->message->text();
            break;
        case QtCriticalMsg:
            qCCritical(c) << ui->message->text();
            break;
        case QtFatalMsg:
            qFatal(ui->message->text().toUtf8().constData());
            break;
        case QtInfoMsg:
            qCInfo(c) << ui->message->text();
            break;
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
