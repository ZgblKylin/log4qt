#ifndef LOG4QTFILESAVETASK_H
#define LOG4QTFILESAVETASK_H

#include <QtCore/QMutex>
#include <QtCore/QDir>
#include <QtCore/QTextCodec>
#include <log4qt.h>

class QFile;
class QThread;

// base class for save log messages into file
class log4qtFileSaveTask : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString dir READ getDir WRITE setDir)
    Q_PROPERTY(qint64 maxFileSize MEMBER maxFileSize)
    Q_PROPERTY(QString pattern MEMBER pattern)
    Q_PROPERTY(int filter MEMBER filter)
public:
    explicit log4qtFileSaveTask(const QDir& defaultDir, const QString& category, QObject* parent = 0);
    virtual ~log4qtFileSaveTask();

    // record log message into file
    void record(const QSharedPointer<log4qt::impl::LogMessage> message);

protected:
    QString getDir() const;
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
    QDir dir; // log file dir, default is ./Log
    qint64 maxFileSize = 100 * 1024 * 1024; // maximum size for single log file
    QString pattern = log4qt::impl::DefaultPattern; // log pattern
    int filter = log4qt::impl::DefaultFilter; // message with level under will not be recorded

    QString category;
    mutable QMutex mutex;
    QTextCodec* codec = QTextCodec::codecForName("UTF-8"); // log file codec, UTF-8 for default
    QThread* thread;    // log file save will be run in separate thread
    QString fileName;   // log file name
    int fileIndex = 1;  // current file index, used for preventing duplication of name
};

// save log mesasge into QFile
class log4qtFileNormalSaveTask : public log4qtFileSaveTask
{
    Q_OBJECT
    Q_PROPERTY(int flushCount MEMBER flushCount)
public:
    explicit log4qtFileNormalSaveTask(const QDir& defaultDir, const QString& category, QObject* parent = 0);

private:
    // log4qtFileSaverBase interface
    virtual void save(const QByteArray& text) override final;

    int flushCount = -1;    // call flush when flushCount messages written
    int count = 0;          // current written count
};

// save log message by mmap
class log4qtFileMmapSaveTask : public log4qtFileSaveTask
{
    Q_OBJECT
    Q_PROPERTY(qint64 mapSize MEMBER mapSize)
public:
    explicit log4qtFileMmapSaveTask(const QDir& defaultDir, const QString& category, QObject* parent = 0);

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

#endif // LOG4QTFILESAVETASK_H
