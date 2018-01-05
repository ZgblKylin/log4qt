#ifndef LOG4QTDEBUGOUTPUT_H
#define LOG4QTDEBUGOUTPUT_H

#include <log4qt.h>

// send log message into qDebug stream, using Qt::DirectConnection
class LOG4QTSHARED_EXPORT log4qtDebugOutput : public log4qt::impl::ILogProcessor
{
    Q_OBJECT
    Q_PROPERTY(QString pattern MEMBER pattern)
    Q_PROPERTY(int filter MEMBER filter)
    Q_PROPERTY(bool mapToQt MEMBER mapToQt)
public:
    explicit log4qtDebugOutput(QObject* parent = 0);
    virtual ~log4qtDebugOutput();

    // ILogProcessor interface
    virtual void start() override;
    Q_SLOT virtual void log(const QSharedPointer<log4qt::impl::LogMessage> message) override;

private:
    // log pattern
    QString pattern = QStringLiteral("[%1] %2")
                      .arg(log4qt::impl::PatternCategory)
                      .arg(log4qt::impl::DefaultPattern);

    // message with level under filter will not be processed
    int filter = log4qt::impl::DefaultFilter;

    // if true, message type of Warning/Critical/Fatal will be printed by qWarning/qCritical/qFatal
    bool mapToQt = false;
};

#endif // LOG4QTDEBUGOUTPUT_H
