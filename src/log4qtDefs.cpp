#include <QtCore/QMutexLocker>
#include <QtCore/QCoreApplication>
#include <QtCore/QGlobalStatic>
#include <log4qt.h>
#include "log4qtLogEngine.h"

namespace log4qt {
namespace impl {
/* ================ Log Pattern Functions ================ */
QString& parsePattern(QString& pattern)
{
    pattern.replace(PatternDateTime, PlaceHolderDateTime);
    pattern.replace(PatternType, PlaceHolderType);
    pattern.replace(PatternLevel, PlaceHolderLevel);
    pattern.replace(PatternPID, PlaceHolderPID);
    pattern.replace(PatternThreadId, PlaceHolderThreadId);
    pattern.replace(PatternThreadPtr, PlaceHolderThreadPtr);
    pattern.replace(PatternFile, PlaceHolderFile);
    pattern.replace(PatternLine, PlaceHolderLine);
    pattern.replace(PatternFunction, PlaceHolderFunction);
    pattern.replace(PatternMessage, PlaceHolderMessage);
    return pattern;
}

QString processMessage(const QString& parsedPattern, const QSharedPointer<LogMessage>& message)
{
    QString text = parsedPattern;
    text.replace(PlaceHolderDateTime, message->dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss zzz")));
    text.replace(PlaceHolderType, getLogTypeString(message->type));
    text.replace(PlaceHolderLevel, QString::number(message->level));
    text.replace(PlaceHolderPID, QString::number(qApp->applicationPid()));
    text.replace(PlaceHolderThreadId, QString::number(quintptr(message->threadid)));
    text.replace(PlaceHolderThreadPtr, QStringLiteral("0x%1").arg(quintptr(message->threadptr), sizeof(quintptr)*2, 0, QLatin1Char('0')));
    text.replace(PlaceHolderFile, message->file);
    text.replace(PlaceHolderLine, QString::number(message->line));
    text.replace(PlaceHolderFunction, message->function);
    text.replace(PlaceHolderMessage, *(message->message));
    return text;
}


/* ================ LogType Functions ================ */
LogType getLogType(int level)
{
    if(level < LogType::Infomation)
        return LogType::Debug;
    else if(level < LogType::Warning)
        return LogType::Infomation;
    else if(level < LogType::Critical)
        return LogType::Warning;
    else if(level < LogType::Fatal)
        return LogType::Critical;
    else
        return LogType::Fatal;
}

QString getLogTypeString(LogType type)
{
    switch(type)
    {
    case LogType::Debug:
        return QObject::tr("Debug");
    case LogType::Infomation:
        return QObject::tr("Infomation");
    case LogType::Warning:
        return QObject::tr("Warning");
    case LogType::Critical:
        return QObject::tr("Critical");
    case LogType::Fatal:
        return QObject::tr("Fatal");
    }
    return QString();
}


/* ================ LogMessage Implementation ================ */
LogMessage::LogMessage(int lvl, QString fl, int ln, QString fn)
    : dateTime(QDateTime::currentDateTime()),
      type(getLogType(lvl)),
      level(lvl),
      pid(qApp->applicationPid()),
      threadid(QThread::currentThreadId()),
      threadptr(QThread::currentThread()),
      file(fl),
      line(ln),
      function(fn),
      message(QSharedPointer<QString>::create()) {}


/* ================ LogEngine Implementation ================ */
Q_GLOBAL_STATIC(LogEngineImpl, engine)

LogStream LogEngine::log(int logLevel, const QString& file, int line, const QString& function)
{
    return engine->log(logLevel, file, line, function);
}

bool LogEngine::registerProcessor(ILogProcessorBase* processor, Qt::ConnectionType connectionType)
{
    return engine->registerProcessor(processor, connectionType);
}

void LogEngine::unRegisterProcessor(ILogProcessorBase* processor)
{
    return engine->unRegisterProcessor(processor);
}


/* ================ LogStream Implementation ================ */
LogStream::LogStream(QSharedPointer<LogMessage> msg, std::function<void(QSharedPointer<LogMessage>)> onDel)
    : QDebug(msg->message.data()),
      message(msg),
      onDelete(onDel) {}

LogStream::~LogStream()
{
    onDelete(message);
}

LogStream& LogStream::log()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    this->noquote();
#endif
    return *this;
}

void LogStream::log(const char* format, ...)
{
    std::va_list args;
    va_start(args, format);
    int size = std::vsnprintf(nullptr, 0, format, args);
    std::vector<char> buf(size + 1, '\0'); // note +1 for null terminator
    std::vsnprintf(buf.data(), buf.size(), format, args);
    va_end(args);
    (*this) << buf.data();
}

} // namespace impl
} // namespace log4qt
