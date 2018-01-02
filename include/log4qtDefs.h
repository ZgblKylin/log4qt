#ifndef LOG4QTDEFS_H
#define LOG4QTDEFS_H

#include <QtCore/QtGlobal>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <functional>
#include <vector>
#include <cstdarg>

#if defined(LOG4QT_LIBRARY)
#  define LOG4QTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LOG4QTSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace log4qt {
namespace impl {
struct LogMessage;

// patterns for formating log message
static const QString DateTime = QStringLiteral("%{datetime}");
static const QString Type = QStringLiteral("%{type}");
static const QString Level = QStringLiteral("%{level}");
static const QString PID = QStringLiteral("%{pid}");
static const QString ThreadId = QStringLiteral("%{threadid}");
static const QString ThreadPtr = QStringLiteral("%{qthreadptr}");
static const QString File = QStringLiteral("%{file}");
static const QString Line = QStringLiteral("%{line}");
static const QString Function = QStringLiteral("%{function}");
static const QString Message = QStringLiteral("%{message}");
static const QString DefaultPattern = QStringLiteral("%1 %2(%3) %4:%5 %6:%7 %8 - %9")
                                      .arg(DateTime)
                                      .arg(Type)
                                      .arg(Level)
                                      .arg(PID)
                                      .arg(ThreadId)
                                      .arg(File)
                                      .arg(Line)
                                      .arg(Function)
                                      .arg(Message);

// parse the message pattern, combine LogMessage into return text
QString LOG4QTSHARED_EXPORT processMessage(const QString& pattern, const QSharedPointer<LogMessage>& message);

// log type, calculated by LogLevel
enum LogType
{
    Debug = 0,
    Infomation = 25,
    Warning = 50,
    Critical = 75,
    Fatal = 100
};
static const int DefaultFilter = LogType::Infomation;
// get LogType by input log level
LogType LOG4QTSHARED_EXPORT getLogType(int level);
// get printable description string for LogType
QString LOG4QTSHARED_EXPORT getLogTypeString(LogType type);

// log message
struct LogMessage
{
    QDateTime dateTime;
    LogType type;
    int level;
    qint64 pid;
    Qt::HANDLE threadid;
    QThread* threadptr;
    QString file;
    int line;
    QString function;
    QSharedPointer<QString> message;
    LogMessage(int level, QString file, int line, QString function);
};

class LogStream;
struct ILogProcessorBase;
// interface for log engine
namespace LogEngine
{
    // generate LogStream just like QMessageLogger
    LOG4QTSHARED_EXPORT LogStream log(int logLevel, const QString& file, int line, const QString& function);

    // register log processor, pushing log messages using connectionType
    // WARNING
    //     When processor and Logger are in the same thread,
    //     Qt::BlockingQueuedConnection will be replaced by Qt::DirectConnection to prevent deadlock.
    LOG4QTSHARED_EXPORT bool registerProcessor(ILogProcessorBase* processor, Qt::ConnectionType connectionType);

    // unregister log processor
    LOG4QTSHARED_EXPORT void unRegisterProcessor(ILogProcessorBase* processor);
} // namespace LogEngine

// Interface for log processor
struct ILogProcessorBase
{
    virtual ~ILogProcessorBase() { LogEngine::unRegisterProcessor(this); }

    // callback for pushing log message
    // must explicitly declared as Q_SLOT
    virtual void log(const QSharedPointer<LogMessage> message) = 0;
};
template<class T, class U>
struct ILogProcessorBaseWrapper : public ILogProcessorBase, public T
{
    explicit ILogProcessorBaseWrapper(U* parent = 0) : T(parent) {}
    virtual ~ILogProcessorBaseWrapper() {}
};
using ILogProcessor = ILogProcessorBaseWrapper<QObject, QObject>;
template<typename T>
using ILogProcessorWidget = ILogProcessorBaseWrapper<T, QWidget>;

// log output stream, has the same feature as QDebug
class LOG4QTSHARED_EXPORT LogStream : public QDebug
{
public:
    LogStream(QSharedPointer<LogMessage> msg, std::function<void(QSharedPointer<LogMessage>)> onDel);
    ~LogStream();

    // C++ style output interface, equivalent to "QDebug() << ..."
    // will call noquote() when Qt version >= 5.4.0
    LogStream& log();

    // C style output interface, equivalent to "qDebug(format, ...)"
    void log(const char* format, ...) Q_ATTRIBUTE_FORMAT_PRINTF(2, 3);

private:
    // log message
    QSharedPointer<LogMessage> message;
    // callback on destruction, pushing message to LogEngine
    std::function<void(QSharedPointer<LogMessage>)> onDelete;
};

} // namesapce impl
} // namespace log4qt

Q_DECLARE_METATYPE(QSharedPointer<log4qt::impl::LogMessage>)

#endif // LOG4QTDEFS_H
