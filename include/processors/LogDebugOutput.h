#pragma once

#include "../log4qt.h"

namespace log4qt {
class LogDebugOutputPrivate;

// Send log message into Qt debug stream, using Qt::DirectConnection
class LOG4QTSHARED_EXPORT LogDebugOutput : public QObject, public log4qt::ILogProcessor
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LogDebugOutput)
public:
    // Display pattern, default is log4qt::LogMessage::DefaultPattern
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern)
    // Type under filter will not be processed, default is QtWarningMsg
    Q_PROPERTY(QtMsgType filter READ filter WRITE setFilter)

    explicit LogDebugOutput(QObject* parent = nullptr);
    virtual ~LogDebugOutput() override final;

    QString pattern() const;
    void setPattern(const QString& pattern);

    QtMsgType filter() const;
    void setFilter(QtMsgType filter);

protected:
    // ILogProcessor interface
    virtual void log(QtMsgType type, const QMessageLogContext& context, const QString& buf) override final;

    LogDebugOutputPrivate* d_ptr;
};
} // namespace log4qt
