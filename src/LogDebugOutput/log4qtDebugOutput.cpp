#include <processors/log4qtDebugOutput.h>


log4qtDebugOutput::log4qtDebugOutput(QObject* parent)
    : log4qt::impl::ILogProcessor(parent)
{
    log4qt::impl::LogEngine::registerProcessor(this, Qt::DirectConnection);
}

log4qtDebugOutput::~log4qtDebugOutput()
{
}

void log4qtDebugOutput::log(const QSharedPointer<log4qt::impl::LogMessage> message)
{
    if(message->level < filter)
        return;

    const QString text = log4qt::impl::processMessage(pattern, message);
    if(mapToQt)
    {
        switch(message->type)
        {
        case log4qt::impl::Debug:
            qDebug() << text;
            break;
        case log4qt::impl::Infomation:
            qDebug() << text;
            break;
        case log4qt::impl::Warning:
            qWarning() << text;
            break;
        case log4qt::impl::Critical:
            qCritical() << text;
            break;
        case log4qt::impl::Fatal:
            qFatal("%s", text.toUtf8().constData());
            break;
        }
    }
    else
    {
        qDebug() << text.toUtf8().constData();
    }
}
