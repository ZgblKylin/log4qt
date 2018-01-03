#ifndef LOG4QTFILESAVER_H
#define LOG4QTFILESAVER_H

#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <log4qt.h>

// base class for save log messages into file
class LOG4QTSHARED_EXPORT log4qtFileSaverBase : public log4qt::impl::ILogProcessor
{
    Q_OBJECT
    Q_PROPERTY(QDir dir MEMBER dir)
    Q_PROPERTY(qint64 maxFileSize MEMBER maxFileSize)
    Q_PROPERTY(QString pattern MEMBER pattern)
    Q_PROPERTY(int filter MEMBER filter)
public:
    explicit log4qtFileSaverBase(QObject* parent = 0);
    virtual ~log4qtFileSaverBase();

protected:
    // refresh log file pointer, e.g. reopen new file when file.size() > maxFileSize
    void refreshFile();
    void closeFile();

    QFile* file;        // log file
    QTextCodec* codec;  // log file codec, UTF-8 for default
    QMutex mutex;       // mutex to guard file operation
    QString pattern = log4qt::impl::DefaultPattern; // log pattern
    int filter = log4qt::impl::DefaultFilter; // message with level under filter will not be processed

    QThread thread;     // log file save will be run in separate thread
    QDir dir;           // log file dir, default is ./Log
    qint64 maxFileSize; // maximum size for single log file, default is 100MB
    QString fileName;   // log file name
    int fileIndex = 1;  // current file index, used for preventing duplication of name
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
    // ILogProcessor interface
    Q_SLOT virtual void log(const QSharedPointer<log4qt::impl::LogMessage> message) override final;

    int flushCount = -1;    // call flush when flushCount messages written
    int count = 0;          // current written count
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
    // ILogProcessor interface
    Q_SLOT virtual void log(const QSharedPointer<log4qt::impl::LogMessage> message) override final;

    // refresh mmap block
    void map();

    qint64 mapSize;             // mmap block size
    qint64 mapPos = 0;          // current mmap pos
    uchar* block = nullptr;     // current mmap block
    qint64 availableSize = 0;   // available size of current mmap block
};

#endif // LOG4QTFILESAVER_H
