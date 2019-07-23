#pragma once

#include <processors/LogFileSaver.h>
#include "LogFileSaveTask.h"
#include "../Common.h"

namespace log4qt {
class LogFileSaverBase : public QObject
{
    Q_OBJECT
public:
    Q_SIGNAL void record(const QSharedPointer<LogMessage> message);
};

// base class for save log messages into file
class LogFileSaverPrivate
{
public:
    LogFileSaverPrivate(QObject* parent) : q_ptr(parent) {}
    virtual ~LogFileSaverPrivate() = default;

    // ILogProcessor interface
    void log(QSharedPointer<LogMessage>&& message);

    // get task of given category, has same properties as saver
    QObject* getTask(const QString& category) const;

    // create save task for input category
    virtual LogFileSaveTask* createTask(const QString& category) = 0;

    QString dir() const;
    void setDir(const QString& path);

    qint64 maxFileSize() const;
    void setMaxFileSize(qint64 size);

    QString pattern() const;
    void setPattern(const QString& pattern);

    QtMsgType filter() const;
    void setFilter(QtMsgType filter);

    void setAllTaskProperty(const char* name, const QVariant& value);

    QHash<QString, LogFileSaveTask*> tasks; // file save task for each category
    mutable QMutex mutex;

    // properties
    bool dirInited = false;
    QDir dr; // dir to save log files, each category uses a subdir. Default is ./Log
    qint64 maxSize = 100 * 1024 * 1024; // maximum size for single log file
    QString pttrn = log4qt::DefaultPattern; // log pattern
    QtMsgType fltr = QtWarningMsg; // message with level under will not be recorded

    LogFileSaverBase base;
    QObject* q_ptr;
};

class LogFileNormalSaverPrivate : public LogFileSaverPrivate
{
    Q_DECLARE_PUBLIC(LogFileNormalSaver)
public:
    LogFileNormalSaverPrivate(QObject* parent) : LogFileSaverPrivate(parent) {}

    // ILogProcessor interface
    virtual LogFileSaveTask* createTask(const QString& category) override final;

    int flushCount = -1;
};

class LogFileMmapSaverPrivate : public LogFileSaverPrivate
{
    Q_DECLARE_PUBLIC(LogFileMmapSaver)
public:
    LogFileMmapSaverPrivate(QObject* parent) : LogFileSaverPrivate(parent) {}

    // ILogProcessor interface
    virtual LogFileSaveTask* createTask(const QString& category) override final;

    qint64 mapSize = 1024 * 1024;
};
} // namespace log4qt
