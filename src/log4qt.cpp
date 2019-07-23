#include <log4qt.h>
#include "Common.h"

namespace log4qt {
/* ================ LogEngine Implementation ================ */
class LogEnginePrivate
{
public:
    LogEnginePrivate();

    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& buf);
    void handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& buf);

    mutable QMutex mutex;
    QVector<ILogProcessor*> processors;
};
Q_GLOBAL_STATIC(LogEnginePrivate, engine)

LogEnginePrivate::LogEnginePrivate()
    : mutex(QMutex::Recursive)
{
    qRegisterMetaType<QtMsgType>();
    qRegisterMetaType<log4qt::LogMessage>();
    qRegisterMetaType<QSharedPointer<log4qt::LogMessage>>();
    qInstallMessageHandler(messageHandler);
}

void LogEnginePrivate::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& buf)
{
    engine->handleMessage(type, context, buf);
}

void LogEnginePrivate::handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& buf)
{
    while (!mutex.tryLock()) {}
    defer [this]{ mutex.unlock(); };
    for (ILogProcessor* processor : processors) {
        processor->log(type, context, buf);
    }
}

namespace LogEngine {
void registerProcessor(ILogProcessor* processor)
{
    while (!engine->mutex.tryLock()) {}
    defer []{ engine->mutex.unlock(); };
    if (!engine->processors.contains(processor)) {
        engine->processors.append(processor);
    }
}

void unRegisterProcessor(ILogProcessor* processor)
{
    while (!engine->mutex.tryLock()) {}
    defer []{ engine->mutex.unlock(); };
    int index = engine->processors.indexOf(processor);
    if (index >= 0) {
        engine->processors.remove(index);
    }
}
} // namespace LogEngine
/* ================ LogEngine Implementation ================ */

/* ================ LogEngine Implementation ================ */
LogMessage::LogMessage()
    : LogMessage(QtDebugMsg, QMessageLogContext(), QString())
{
}

LogMessage::LogMessage(QtMsgType t, const QMessageLogContext& context, const QString& buf)
    : appName(qApp->applicationName()),
      category(context.category),
      dateTime(QDateTime::currentDateTime()),
      type(t),
      pid(qApp->applicationPid()),
      threadId(QThread::currentThreadId()),
      threadPtr(QThread::currentThread()),
      file(context.file),
      line(context.line),
      function(context.function),
      message(buf)
{
}

LogMessage::LogMessage(const LogMessage& other)
{
    *this = other;
}

LogMessage::LogMessage(LogMessage&& other)
{
    *this = std::forward<LogMessage>(other);
}

LogMessage& LogMessage::operator=(const LogMessage& other)
{
    if (this != &other)
    {
        appName = other.appName;
        category = other.category;
        dateTime = other.dateTime;
        type = other.type;
        pid = other.pid;
        threadId = other.threadId;
        threadPtr = other.threadPtr;
        file = other.file;
        line = other.line;
        function = other.function;
        message = other.message;
    }
    return *this;
}

LogMessage& LogMessage::operator=(LogMessage&& other)
{
    appName.swap(other.appName);
    category.swap(other.category);
    dateTime.swap(other.dateTime);
    type = other.type;
    pid = other.pid;
    threadId = other.threadId;
    threadPtr = other.threadPtr;
    file.swap(other.file);
    line = other.line;
    function.swap(other.function);
    message.swap(other.message);
    return *this;
}
/* ================ LogEngine Implementation ================ */

QString typeToString(QtMsgType type)
{
    switch(type)
    {
    case QtMsgType::QtDebugMsg:
        return QObject::tr("Debug");
    case QtMsgType::QtWarningMsg:
        return QObject::tr("Warning");
    case QtMsgType::QtCriticalMsg:
        return QObject::tr("Critical");
    case QtMsgType::QtFatalMsg:
        return QObject::tr("Fatal");
    case QtMsgType::QtInfoMsg:
        return QObject::tr("Information");
    }
    return QString();
}

QString formatMessage(const QString& pattern, QtMsgType type, const QMessageLogContext& context, const QString& buf)
{
    QString text = pattern;
    text.replace(PatternAppName, qApp->applicationName());
    text.replace(PatternCategory, context.category);
    text.replace(PatternDateTime, QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss zzz")));
    text.replace(PatternType, typeToString(type));
    text.replace(PatternPID, QString::number(qApp->applicationPid()));
    text.replace(PatternThreadId, QString::number(quintptr(QThread::currentThreadId())));
    text.replace(PatternThreadPtr, QStringLiteral("0x%1").arg(quintptr(QThread::currentThread()), sizeof(quintptr)*2, 16, QLatin1Char('0')));
    text.replace(PatternFile, context.file);
    text.replace(PatternLine, QString::number(context.line));
    text.replace(PatternFunction, context.function);
    text.replace(PatternMessage, buf);
    return text;
}

QString formatMessage(const QString& pattern, const LogMessage& message)
{
    QString text = pattern;
    text.replace(PatternAppName, message.appName);
    text.replace(PatternCategory, message.category);
    text.replace(PatternDateTime, message.dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss zzz")));
    text.replace(PatternType, typeToString(message.type));
    text.replace(PatternPID, QString::number(message.pid));
    text.replace(PatternThreadId, QString::number(quintptr(message.threadId)));
    text.replace(PatternThreadPtr, QStringLiteral("0x%1").arg(quintptr(message.threadPtr), sizeof(quintptr)*2, 16, QLatin1Char('0')));
    text.replace(PatternFile, message.file);
    text.replace(PatternLine, QString::number(message.line));
    text.replace(PatternFunction, message.function);
    text.replace(PatternMessage, message.message);
    return text;
}
} // namespace log4qt
