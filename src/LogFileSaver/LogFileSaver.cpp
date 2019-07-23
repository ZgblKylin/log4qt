#include "LogFileSaverPrivate.h"

namespace log4qt {
/* ================ LogFileSaverPrivate ================ */
QObject* LogFileSaverPrivate::getTask(const QString& category) const
{
    return tasks.value(category, nullptr);
}

void LogFileSaverPrivate::log(QSharedPointer<LogMessage>&& message)
{
    while(!mutex.tryLock()) {}
    defer [this]{ mutex.unlock(); };
    if(Q_UNLIKELY(!dirInited))
    {
        setDir(qApp->applicationDirPath()
                      + QDir::separator()
                      + QStringLiteral("Log"));
        dirInited = true;
    }
    LogFileSaveTask* task = qobject_cast<LogFileSaveTask*>(getTask(message->category));
    if(!task)
    {
        task = createTask(message->category);
        task->setObjectName(q_ptr->objectName() + '_' + message->category);
        task->setProperty("dir", dr.absoluteFilePath(message->category));
        task->setProperty("maxFileSize", maxSize);
        task->setProperty("pattern", pttrn);
        task->setProperty("filter", fltr);
        tasks.insert(message->category, task);
    }
    emit base.record(message);
}

QString LogFileSaverPrivate::dir() const
{
    return dr.absolutePath();
}

void LogFileSaverPrivate::setDir(const QString& path)
{
    while(!mutex.tryLock()) {}
    defer [this]{ mutex.unlock(); };
    dirInited = true;
    dr.setPath(path);
    if(!dr.exists()) dr.mkpath(dr.absolutePath());
    for(auto it = tasks.cbegin(); it != tasks.cend(); ++it) {
        it.value()->setProperty("dir", dr.absoluteFilePath(it.key()));
    }
}

qint64 LogFileSaverPrivate::maxFileSize() const
{
    return maxSize;
}

void LogFileSaverPrivate::setMaxFileSize(qint64 size)
{
    maxSize = size;
    setAllTaskProperty("maxFileSize", maxSize);
}

QString LogFileSaverPrivate::pattern() const
{
    return pttrn;
}

void LogFileSaverPrivate::setPattern(const QString& pattern)
{
    pttrn = pattern;
    setAllTaskProperty("pattern", pttrn);
}

QtMsgType LogFileSaverPrivate::filter() const
{
    return fltr;
}

void LogFileSaverPrivate::setFilter(QtMsgType filter)
{
    fltr = filter;
    setAllTaskProperty("filter", fltr);
}

void LogFileSaverPrivate::setAllTaskProperty(const char* name, const QVariant& value)
{
    for(auto it = tasks.begin(); it != tasks.end(); ++it) {
        it.value()->setProperty(name, value);
    }
}
/* ================ LogFileSaverPrivate ================ */

/* ================ LogFileNormalSaver ================ */
LogFileSaveTask* LogFileNormalSaverPrivate::createTask(const QString& category)
{
    Q_Q(LogFileNormalSaver);
    auto task = new LogFileNormalSaveTask(dir(), category, q);
    task->setProperty("flushCount", flushCount);
    QObject::connect(&base, &LogFileSaverBase::record,
                     task, &LogFileSaveTask::record,
                     Qt::QueuedConnection);
    return task;
}

LogFileNormalSaver::LogFileNormalSaver(QObject *parent)
    : QObject(parent),
      d_ptr(new LogFileNormalSaverPrivate(this))
{
}

QString LogFileNormalSaver::dir() const
{
    Q_D(const LogFileNormalSaver);
    return d->dir();
}

void LogFileNormalSaver::setDir(const QString& path)
{
    Q_D(LogFileNormalSaver);
    d->setDir(path);
}

qint64 LogFileNormalSaver::maxFileSize() const
{
    Q_D(const LogFileNormalSaver);
    return d->maxFileSize();
}

void LogFileNormalSaver::setMaxFileSize(qint64 size)
{
    Q_D(LogFileNormalSaver);
    d->setMaxFileSize(size);
}

QString LogFileNormalSaver::pattern() const
{
    Q_D(const LogFileNormalSaver);
    return d->pattern();
}

void LogFileNormalSaver::setPattern(const QString& pattern)
{
    Q_D(LogFileNormalSaver);
    d->setPattern(pattern);
}

QtMsgType LogFileNormalSaver::filter() const
{
    Q_D(const LogFileNormalSaver);
    return d->filter();
}

void LogFileNormalSaver::setFilter(QtMsgType filter)
{
    Q_D(LogFileNormalSaver);
    d->setFilter(filter);
}

int LogFileNormalSaver::flushCount() const
{
    Q_D(const LogFileNormalSaver);
    return d->flushCount;
}

void LogFileNormalSaver::setFlushCount(int count)
{
    Q_D(LogFileNormalSaver);
    d->flushCount = count;
    d->setAllTaskProperty("flushCount", d->flushCount);
}

void LogFileNormalSaver::log(QtMsgType type, const QMessageLogContext& context, const QString& buf)
{
    Q_D(LogFileNormalSaver);
    d->log(QSharedPointer<LogMessage>::create(type, context, buf));
}

/* ================ LogFileMmapSaver ================ */
LogFileSaveTask* LogFileMmapSaverPrivate::createTask(const QString& category)
{
    Q_Q(LogFileMmapSaver);
    auto task = new LogFileMmapSaveTask(dir(), category, q);
    task->setProperty("mapSize", mapSize);
    QObject::connect(&base, &LogFileSaverBase::record,
                     task, &LogFileSaveTask::record,
                     Qt::DirectConnection);
    return task;
}

LogFileMmapSaver::LogFileMmapSaver(QObject *parent)
    : QObject(parent),
      d_ptr(new LogFileMmapSaverPrivate(parent))
{
}

QString LogFileMmapSaver::dir() const
{
    Q_D(const LogFileMmapSaver);
    return d->dir();
}

void LogFileMmapSaver::setDir(const QString& path)
{
    Q_D(LogFileMmapSaver);
    d->setDir(path);
}

qint64 LogFileMmapSaver::maxFileSize() const
{
    Q_D(const LogFileMmapSaver);
    return d->maxFileSize();
}

void LogFileMmapSaver::setMaxFileSize(qint64 size)
{
    Q_D(LogFileMmapSaver);
    d->setMaxFileSize(size);
}

QString LogFileMmapSaver::pattern() const
{
    Q_D(const LogFileMmapSaver);
    return d->pattern();
}

void LogFileMmapSaver::setPattern(const QString& pattern)
{
    Q_D(LogFileMmapSaver);
    d->setPattern(pattern);
}

QtMsgType LogFileMmapSaver::filter() const
{
    Q_D(const LogFileMmapSaver);
    return d->filter();
}

void LogFileMmapSaver::setFilter(QtMsgType filter)
{
    Q_D(LogFileMmapSaver);
    d->setFilter(filter);
}

qint64 LogFileMmapSaver::mapSize() const
{
    Q_D(const LogFileMmapSaver);
    return d->mapSize;
}

void LogFileMmapSaver::setMapSize(qint64 size)
{
    Q_D(LogFileMmapSaver);
    d->mapSize = size;
    d->setAllTaskProperty("mapSize", d->mapSize);
}

void LogFileMmapSaver::log(QtMsgType type, const QMessageLogContext& context, const QString& buf)
{
    Q_D(LogFileMmapSaver);
    d->log(QSharedPointer<LogMessage>::create(type, context, buf));
}
/* ================ LogFileMmapSaver ================ */
} // namespasce log4qt
