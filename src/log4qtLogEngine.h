#ifndef LOG4QTLOGENGINE_H
#define LOG4QTLOGENGINE_H

#include <log4qt.h>
#include <QtCore/QQueue>
#include <QtCore/QMutex>

class LogEngineImpl : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(LogEngineImpl)
public:
    LogEngineImpl();
    ~LogEngineImpl() = default;

    log4qt::impl::LogStream log(int logLevel, const QString& file, int line, const QString& function);

    bool registerProcessor(log4qt::impl::ILogProcessorBase* processor, Qt::ConnectionType connectionType);
    void unRegisterProcessor(log4qt::impl::ILogProcessorBase* processor);

private:
    Q_SIGNAL void newLog(QSharedPointer<log4qt::impl::LogMessage> message) const;

    QMutex mutex;
    QQueue<log4qt::impl::ILogProcessorBase*> processors;
};

#endif // LOG4QTLOGENGINE_H
