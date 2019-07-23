#include <processors/LogDebugOutput.h>

namespace log4qt {
class LogDebugOutputPrivate
{
    Q_DECLARE_PUBLIC(LogDebugOutput)
    LogDebugOutput* q_ptr;

public:
    LogDebugOutputPrivate(LogDebugOutput* parent) : q_ptr(parent) {}

    QString pattern = DefaultPattern;
    QtMsgType filter = QtWarningMsg;
    QtMessageHandler defaultHandler;
};

LogDebugOutput::LogDebugOutput(QObject* parent)
    : QObject(parent),
      d_ptr(new LogDebugOutputPrivate(this))
{
    Q_D(LogDebugOutput);
    QtMessageHandler handler = qInstallMessageHandler(nullptr);
    d->defaultHandler = qInstallMessageHandler(handler);
}

LogDebugOutput::~LogDebugOutput()
{
    delete d_ptr;
}

QString LogDebugOutput::pattern() const
{
    Q_D(const LogDebugOutput);
    return d->pattern;
}

void LogDebugOutput::setPattern(const QString& ptn)
{
    Q_D(LogDebugOutput);
    d->pattern = ptn;
}

QtMsgType LogDebugOutput::filter() const
{
    Q_D(const LogDebugOutput);
    return d->filter;
}

void LogDebugOutput::setFilter(QtMsgType fltr)
{
    Q_D(LogDebugOutput);
    d->filter = fltr;
}

void LogDebugOutput::log(QtMsgType type, const QMessageLogContext& context, const QString& buf)
{
    Q_D(LogDebugOutput);
    if(type < d->filter) {
        return;
    }
    d->defaultHandler(type, context,
                      formatMessage(d->pattern, type, context, buf));
}
} // namesapce log4qt
