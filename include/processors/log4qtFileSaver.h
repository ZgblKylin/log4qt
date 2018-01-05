#ifndef LOG4QTFILESAVER_H
#define LOG4QTFILESAVER_H

#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <log4qt.h>

class log4qtFileSaveTask;

// base class for save log messages into file
class LOG4QTSHARED_EXPORT log4qtFileSaverBase : public log4qt::impl::ILogProcessor
{
    Q_OBJECT
    /* ======== All properties will be set to all tasks, replacing their own value ======== */
    Q_PROPERTY(QString dir READ getDir WRITE setDir) // absolute path
    Q_PROPERTY(qint64 maxFileSize READ getMaxFileSize WRITE setMaxFileSize)
    Q_PROPERTY(QString pattern READ getPattern WRITE setPattern)
    Q_PROPERTY(int filter READ getFilter WRITE setFilter)
public:
    explicit log4qtFileSaverBase(QObject* parent = 0);
    virtual ~log4qtFileSaverBase() = default;

    // ILogProcessor interface
    virtual void start() override final;
    Q_SLOT virtual void log(const QSharedPointer<log4qt::impl::LogMessage> message) override final;

    // get task of given category, has same properties as saver
    QObject* getTask(const QString& category) const;

protected:
    // create save task for input category
    virtual log4qtFileSaveTask* createTask(const QString& category) const = 0;

    // send query to task for recording message
    Q_SIGNAL void record(const QSharedPointer<log4qt::impl::LogMessage> message);

private:
    QString getDir() const;
    void setDir(const QString& path);

    qint64 getMaxFileSize() const;
    void setMaxFileSize(qint64 value);

    QString getPattern() const;
    void setPattern(const QString& value);

    int getFilter() const;
    void setFilter(int value);

    QMap<QString, log4qtFileSaveTask*> tasks; // file save task for each category
    mutable QMutex mutex;

    // properties
    bool dirInited = false;
    QDir dir; // dir to save log files, each category uses a subdir. Default is ./Log
    qint64 maxFileSize = 100 * 1024 * 1024; // maximum size for single log file
    QString pattern = log4qt::impl::DefaultPattern; // log pattern
    int filter = log4qt::impl::DefaultFilter; // message with level under will not be recorded
};

// save log mesasge into QFile, using Qt::QueuedConnection
// when application crashes, several last log messages will be lost
class LOG4QTSHARED_EXPORT log4qtFileNormalSaver : public log4qtFileSaverBase
{
    Q_OBJECT
    Q_PROPERTY(int flushCount MEMBER flushCount)
public:
    explicit log4qtFileNormalSaver(QObject* parent = 0);

private:
    // log4qtFileSaverBase interface
    virtual log4qtFileSaveTask* createTask(const QString& category) const override final;

    int flushCount = -1; // call flush when flushCount messages written
};

// save log message by mmap, using Qt::DirectConnection
// no message lost when application crashes
class LOG4QTSHARED_EXPORT log4qtFileMmapSaver : public log4qtFileSaverBase
{
    Q_OBJECT
    Q_PROPERTY(qint64 mapSize MEMBER mapSize)
public:
    explicit log4qtFileMmapSaver(QObject* parent = 0);

private:
    // log4qtFileSaverBase interface
    virtual log4qtFileSaveTask* createTask(const QString& category) const override final;

    qint64 mapSize = 1024; // mmap block size
};

#endif // LOG4QTFILESAVER_H
