#pragma once

#include <QtCore/QCoreApplication>
#include <QtTest/QtTest>
#include <log4qt.h>
#include <processors/LogDebugOutput.h>
#include <processors/LogDisplayWidget.h>

class TestProcessor : public QObject, public log4qt::ILogProcessor
{
public:
    int count() const;

protected:
    // ILogProcessor interface
    virtual void log(QtMsgType type, const QMessageLogContext& context, const QString& buf) override;
    int cnt = 0;
};

class Log4QtTest : public QObject
{
    Q_OBJECT

public:
    Log4QtTest();
    ~Log4QtTest();

private slots:
    void engineProfile();

    void typeToString();

    void formatMessage();
    void formatMessageProfileWithQtContext();
    void formatMessageProfileWithLogMessage();

    void logDebugOutput();

    void logDisplayBuffer();

    void logDisplayPageColor_data();
    void logDisplayPageColor();
    void logDisplayPage();

    void logDisplayWidgetColor_data();
    void logDisplayWidgetColor();
    void logDisplayWidget();
};
