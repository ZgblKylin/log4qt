#pragma once

#include <log4qt.h>
#include "../Common.h"

namespace log4qt {
// base class for save log messages into file
class LogFileSaveTask : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString dir READ dir WRITE setDir)
    Q_PROPERTY(qint64 maxFileSize MEMBER maxFileSize)
    Q_PROPERTY(QString pattern MEMBER pattern)
    Q_PROPERTY(QtMsgType filter MEMBER filter)

public:
    explicit LogFileSaveTask(const QDir& defaultDir, const QString& category, QObject* parent = nullptr);
    virtual ~LogFileSaveTask() override;

    // record log message into file
    void record(const QSharedPointer<LogMessage> message);

protected:
    QString dir() const;
    void setDir(const QString& path);

    // save text into file
    virtual void save(const QByteArray& text) = 0;

    QFile* file; // log file

private:
    // refresh log file pointer, e.g. reopen new file when file.size() > maxFileSize
    void refreshFile();
    // close file, remove it if is empty
    void closeFile();

    // properties
    QDir dr; // log file dir, default is ./Log
    qint64 maxFileSize = 100 * 1024 * 1024; // maximum size for single log file
    QString pattern = log4qt::DefaultPattern; // log pattern
    QtMsgType filter = QtWarningMsg; // message with level under will not be recorded

    QString category;
    mutable QMutex mutex;
    QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // log file codec, UTF-8 for default
    QThread* thread;    // log file save will be run in separate thread
    QString fileName;   // log file name
    int fileIndex = 1;  // current file index, used for preventing duplication of name
};

// save log mesasge into QFile
class LogFileNormalSaveTask : public LogFileSaveTask
{
    Q_OBJECT
    Q_PROPERTY(int flushCount MEMBER flushCount)
public:
    explicit LogFileNormalSaveTask(const QDir& defaultDir, const QString& category, QObject* parent = nullptr);

private:
    // log4qtFileSaverBase interface
    virtual void save(const QByteArray& text) override final;

    int flushCount = -1;    // call flush when flushCount messages written
    int count = 0;          // current written count
};

// save log message by mmap
class LogFileMmapSaveTask : public LogFileSaveTask
{
    Q_OBJECT
    Q_PROPERTY(qint64 mapSize MEMBER mapSize)
public:
    explicit LogFileMmapSaveTask(const QDir& defaultDir, const QString& category, QObject* parent = nullptr);

private:
    // log4qtFileSaverBase interface
    virtual void save(const QByteArray& text) override final;

    // refresh mmap block
    void map();

    qint64 mapSize;             // mmap block size
    qint64 mapPos = 0;          // current mmap pos
    uchar* block = nullptr;     // current mmap block
    qint64 availableSize = 0;   // available size of current mmap block
};
} // namespace log4qt
