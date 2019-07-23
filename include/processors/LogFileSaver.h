#pragma once

#include <QtCore/QDir>
#include "../log4qt.h"

namespace log4qt {
class LogFileNormalSaverPrivate;
class LogFileMmapSaverPrivate;

// save log mesasge into QFile, using Qt::QueuedConnection
// when application crashes, several last log messages will be lost
class LOG4QTSHARED_EXPORT LogFileNormalSaver : public QObject, public log4qt::ILogProcessor
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LogFileNormalSaver)

    Q_PROPERTY(QString dir READ dir WRITE setDir) // absolute path
    Q_PROPERTY(qint64 maxFileSize READ maxFileSize WRITE setMaxFileSize) // default 100MB
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern)
    Q_PROPERTY(QtMsgType filter READ filter WRITE setFilter)

    // call flush when flushCount messages written, default is -1(don't call flush)
    Q_PROPERTY(int flushCount READ flushCount WRITE setFlushCount)

public:
    explicit LogFileNormalSaver(QObject* parent = nullptr);

    QString dir() const;
    void setDir(const QString& path);

    qint64 maxFileSize() const;
    void setMaxFileSize(qint64 size);

    QString pattern() const;
    void setPattern(const QString& pattern);

    QtMsgType filter() const;
    void setFilter(QtMsgType filter);

    int flushCount() const;
    void setFlushCount(int count);

protected:
    // ILogProcessor interface
    virtual void log(QtMsgType type, const QMessageLogContext& context, const QString& buf) override final;

private:
    LogFileNormalSaverPrivate* d_ptr;
};

// save log message by mmap, using Qt::DirectConnection
// no message lost when application crashes
class LOG4QTSHARED_EXPORT LogFileMmapSaver : public QObject, public log4qt::ILogProcessor
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LogFileMmapSaver)

    Q_PROPERTY(QString dir READ dir WRITE setDir) // absolute path
    Q_PROPERTY(qint64 maxFileSize READ maxFileSize WRITE setMaxFileSize) // default 100MB
    Q_PROPERTY(QString pattern READ pattern WRITE setPattern)
    Q_PROPERTY(QtMsgType filter READ filter WRITE setFilter)

    // mmap block size, default 1kb
    Q_PROPERTY(qint64 mapSize READ mapSize WRITE setMapSize)

public:
    explicit LogFileMmapSaver(QObject* parent = nullptr);

    QString dir() const;
    void setDir(const QString& path);

    qint64 maxFileSize() const;
    void setMaxFileSize(qint64 size);

    QString pattern() const;
    void setPattern(const QString& pattern);

    QtMsgType filter() const;
    void setFilter(QtMsgType filter);

    qint64 mapSize() const;
    void setMapSize(qint64 size);

protected:
    // ILogProcessor interface
    virtual void log(QtMsgType type, const QMessageLogContext& context, const QString& buf) override final;

private:
    LogFileMmapSaverPrivate* d_ptr;
};
} // namespace log4qt
