#pragma once

#include <QtCore/QDateTime>
#include <QtCore/QtGlobal>
#include <QtCore/QLoggingCategory>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QtDebug>

#if defined(LOG4QT_LIBRARY)
#  define LOG4QTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LOG4QTSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace log4qt {
class LogEnginePrivate;
struct ILogProcessor;
// Interface for log engine, which will take over Qt's logging stream, redirect
// it to registered processors. If no processes are registed, Qt's logging
// stream wil not be redirected.
namespace LogEngine
{
    // Register log processor, pushing log messages using connectionType
    // Will return false in these cases:
    //   - processor already registered
    //   - processor does not implement QObject
    LOG4QTSHARED_EXPORT void registerProcessor(ILogProcessor* processor);

    // Unregister log processor
    LOG4QTSHARED_EXPORT void unRegisterProcessor(ILogProcessor* processor);
};

// Interface for log processor
struct ILogProcessor
{
    virtual ~ILogProcessor() { LogEngine::unRegisterProcessor(this); }

    // register self to engine and start recoding logs
    void start() { LogEngine::registerProcessor(this); }

    // Callback for pushing log message
    virtual void log(QtMsgType type, const QMessageLogContext& context, const QString& buf) = 0;
};

enum Field
{
    AppName,
    Category,
    DateTime,
    Type,
    Pid,
    ThreadId,
    ThreadPtr,
    File,
    Line,
    Function,
    Message
};
struct LOG4QTSHARED_EXPORT LogMessage
{
    LogMessage();
    LogMessage(QtMsgType type, const QMessageLogContext& context, const QString& buf);
    LogMessage(const LogMessage& other);
    LogMessage(LogMessage&& other);

    LogMessage& operator=(const LogMessage& other);
    LogMessage& operator=(LogMessage&& other);

    QString appName;
    QString category;
    QDateTime dateTime;
    QtMsgType type;
    qint64 pid;
    Qt::HANDLE threadId;
    QThread* threadPtr;
    QString file;
    int line;
    QString function;
    QString message;
};

// get printable description string for LogType
LOG4QTSHARED_EXPORT QString typeToString(QtMsgType type);
// patterns for formating log message
static constexpr const char* PatternAppName = "%{appname}";
static constexpr const char* PatternCategory = "%{category}";
static constexpr const char* PatternDateTime = "%{datetime}";
static constexpr const char* PatternType = "%{type}";
static constexpr const char* PatternPID = "%{pid}";
static constexpr const char* PatternThreadId = "%{threadid}";
static constexpr const char* PatternThreadPtr = "%{qthreadptr}";
static constexpr const char* PatternFile = "%{file}";
static constexpr const char* PatternLine = "%{line}";
static constexpr const char* PatternFunction = "%{function}";
static constexpr const char* PatternMessage = "%{message}";
// [Category] DateTime Type AppName(PID):ThreadId File:Line Function - Message
static const QString DefaultPattern = QStringLiteral("[%1:%2] %3 %4(%5):%6 %7:%8 %9 - %10")
                                      .arg(PatternCategory)
                                      .arg(PatternType)
                                      .arg(PatternDateTime)
                                      .arg(PatternAppName)
                                      .arg(PatternPID)
                                      .arg(PatternThreadId)
                                      .arg(PatternFile)
                                      .arg(PatternLine)
                                      .arg(PatternFunction)
                                      .arg(PatternMessage);
// combine LogMessage into return text base on pattern
LOG4QTSHARED_EXPORT QString formatMessage(const QString& pattern, QtMsgType type, const QMessageLogContext& context, const QString& buf);
LOG4QTSHARED_EXPORT QString formatMessage(const QString& pattern, const LogMessage& message);
} // namespace log4qt
Q_DECLARE_METATYPE(QtMsgType)
Q_DECLARE_METATYPE(log4qt::LogMessage)
Q_DECLARE_METATYPE(QSharedPointer<log4qt::LogMessage>)
