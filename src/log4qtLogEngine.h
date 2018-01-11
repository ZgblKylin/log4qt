#ifndef LOG4QTLOGENGINE_H
#define LOG4QTLOGENGINE_H

#include <log4qt.h>
#include <QtCore/QMutex>
#include <QtCore/QVector>
#include <QtCore/QHash>

class LogEngineImpl : public QObject, public log4qt::impl::LogEngine
{
    Q_OBJECT
    Q_DISABLE_COPY(LogEngineImpl)
public:
    LogEngineImpl();
    ~LogEngineImpl() = default;

    virtual log4qt::impl::LogStream log(const QString& category, int logLevel, const QString& file, int line, const QString& function) override final;

    virtual bool registerProcessor(log4qt::impl::ILogProcessorBase* processor, Qt::ConnectionType connectionType) override final;
    virtual void unRegisterProcessor(log4qt::impl::ILogProcessorBase* processor) override final;

    virtual int categoryFilter(const QString& name) override final;
    virtual void setCategoryFilter(const QString& name, int filter) override final;

private:
    Q_SIGNAL void newLog(QSharedPointer<log4qt::impl::LogMessage> message) const;

    mutable QMutex mutex;
    QVector<log4qt::impl::ILogProcessorBase*> processors;
    QHash<QString, int> categories;
};

#endif // LOG4QTLOGENGINE_H
