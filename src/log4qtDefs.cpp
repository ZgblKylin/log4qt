#include <QtCore/QMutexLocker>
#include <QtCore/QCoreApplication>
#include <QtCore/QGlobalStatic>
#include <log4qt.h>

namespace log4qt {
namespace impl {
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
LogMessage::LogMessage(const QString& cg, int lvl, QString fl, int ln, const QString& fn)
    : category(cg),
      dateTime(QDateTime::currentDateTime()),
      type(getLogType(lvl)),
      level(lvl),
      pid(qApp->applicationPid()),
      threadid(QThread::currentThreadId()),
      threadptr(QThread::currentThread()),
      file(fl),
      line(ln),
      function(fn),
      message(QSharedPointer<QString>::create()) {}


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


/* ================ Log Pattern Functions ================ */
QString parsePattern(const QString& pattern)
{
    QString ret = pattern;
    ret.replace(PatternCategory, PlaceHolderCategory);
    ret.replace(PatternDateTime, PlaceHolderDateTime);
    ret.replace(PatternType, PlaceHolderType);
    ret.replace(PatternLevel, PlaceHolderLevel);
    ret.replace(PatternPID, PlaceHolderPID);
    ret.replace(PatternThreadId, PlaceHolderThreadId);
    ret.replace(PatternThreadPtr, PlaceHolderThreadPtr);
    ret.replace(PatternFile, PlaceHolderFile);
    ret.replace(PatternLine, PlaceHolderLine);
    ret.replace(PatternFunction, PlaceHolderFunction);
    ret.replace(PatternMessage, PlaceHolderMessage);
    return ret;
}

QString processMessage(const QString& parsedPattern, const QSharedPointer<LogMessage>& message)
{
    QString text = parsedPattern;
    text.replace(PlaceHolderCategory, message->category);
    text.replace(PlaceHolderDateTime, message->dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss zzz")));
    text.replace(PlaceHolderType, getLogTypeString(message->type));
    text.replace(PlaceHolderLevel, QString::number(message->level));
    text.replace(PlaceHolderPID, QString::number(qApp->applicationPid()));
    text.replace(PlaceHolderThreadId, QString::number(quintptr(message->threadid)));
    text.replace(PlaceHolderThreadPtr, QStringLiteral("0x%1").arg(quintptr(message->threadptr), sizeof(quintptr)*2, 16, QLatin1Char('0')));
    text.replace(PlaceHolderFile, message->file);
    text.replace(PlaceHolderLine, QString::number(message->line));
    text.replace(PlaceHolderFunction, message->function);
    text.replace(PlaceHolderMessage, *(message->message));
    return text;
}

} // namespace impl
} // namespace log4qt
