#include "log4qtLogEngine.h"

log4qt::impl::LogEngine* log4qt::impl::getLogEngine()
{
    static LogEngineImpl instance;
    return &instance;
}

LogEngineImpl::LogEngineImpl()
{
    qRegisterMetaType<QSharedPointer<log4qt::impl::LogMessage>>();
    categories.insert(QString(), log4qt::Infomation);
}

log4qt::impl::LogStream LogEngineImpl::log(const QString& category, int logLevel, const QString& file, int line, const QString& function)
{
    return log4qt::impl::LogStream(QSharedPointer<log4qt::impl::LogMessage>::create(
                                       category,
                                       logLevel,
                                       file,
                                       line,
                                       function),
                                   [this](QSharedPointer<log4qt::impl::LogMessage> msg){ emit newLog(msg); });
}

bool LogEngineImpl::registerProcessor(log4qt::impl::ILogProcessorBase* processor, Qt::ConnectionType connectionType)
{
    QMutexLocker locker(&mutex);

    if(processors.contains(processor)) return false;
    QObject* object = dynamic_cast<QObject*>(processor);
    if(!object) return false;
    processors << processor;

    if((connectionType == Qt::BlockingQueuedConnection)
       && (object->thread() == this->thread()))
        connectionType = Qt::DirectConnection;
    return connect(this, SIGNAL(newLog(QSharedPointer<log4qt::impl::LogMessage>)),
                   object, SLOT(log(QSharedPointer<log4qt::impl::LogMessage>)),
                   Qt::ConnectionType(connectionType | Qt::UniqueConnection));
}

void LogEngineImpl::unRegisterProcessor(log4qt::impl::ILogProcessorBase* processor)
{
    QMutexLocker locker(&mutex);

    QObject* object = dynamic_cast<QObject*>(processor);
    if(object)
        disconnect(this, 0, object, 0);
    processors.removeAll(processor);
}

int LogEngineImpl::categoryFilter(const QString& name)
{
    return categories.value(name, log4qt::Infomation);
}

void LogEngineImpl::setCategoryFilter(const QString& name, int filter)
{
    QMutexLocker locker(&mutex);
    categories[name] = filter;
}
