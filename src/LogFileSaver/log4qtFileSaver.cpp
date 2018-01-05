#include <QCoreApplication>
#include <processors/log4qtFileSaver.h>
#include "log4qtFileSaveTask.h"

/* ================ LogFileSaverBase ================ */
void setAllPageProperty(const char* name, const QVariant& value, QHash<QString, log4qtFileSaveTask*>& tasks)
{
    for(auto it = tasks.begin(); it != tasks.end(); ++ it)
        it.value()->setProperty(name, value);
}

log4qtFileSaverBase::log4qtFileSaverBase(QObject* parent)
    : log4qt::impl::ILogProcessor(parent),
      mutex(QMutex::Recursive)
{
}

void log4qtFileSaverBase::start()
{
    log4qt::impl::getLogEngine()->registerProcessor(this, Qt::DirectConnection);
}

void log4qtFileSaverBase::log(const QSharedPointer<log4qt::impl::LogMessage> message)
{
    QMutexLocker locker(&mutex);
    if(Q_UNLIKELY(!dirInited))
    {
        setDir(qApp->applicationDirPath()
                      + QDir::separator()
                      + QStringLiteral("Log"));
        dirInited = true;
    }
    log4qtFileSaveTask* task = qobject_cast<log4qtFileSaveTask*>(getTask(message->category));
    if(!task)
    {
        task = createTask(message->category);
        task->setObjectName(objectName() + '_' + message->category);
        task->setProperty("dir", dir.absoluteFilePath(message->category));
        task->setProperty("maxFileSize", maxFileSize);
        task->setProperty("pattern", log4qt::impl::parsePattern(pattern));
        task->setProperty("filter", filter);
        tasks.insert(message->category, task);
    }
    return task;
}

QString log4qtFileSaverBase::getDir() const
{
    return dir.absolutePath();
}

void log4qtFileSaverBase::setDir(const QString& path)
{
    QMutexLocker locker(&mutex);
    dirInited = true;
    dir.setPath(path);
    if(!dir.exists())
        dir.mkpath(dir.absolutePath());
    for(auto it = tasks.cbegin(); it != tasks.cend(); ++it)
        it.value()->setProperty("dir", dir.absoluteFilePath(it.key()));
}

qint64 log4qtFileSaverBase::getMaxFileSize() const
{
    return maxFileSize;
}

void log4qtFileSaverBase::setMaxFileSize(qint64 value)
{
    maxFileSize = value;
    setAllPageProperty("maxFileSize", maxFileSize, tasks);
}

QString log4qtFileSaverBase::getPattern() const
{
    return pattern;
}

void log4qtFileSaverBase::setPattern(const QString& value)
{
    pattern = value;
    setAllPageProperty("pattern", log4qt::impl::parsePattern(pattern), tasks);
}

int log4qtFileSaverBase::getFilter() const
{
    return filter;
}

void log4qtFileSaverBase::setFilter(int value)
{
    filter = value;
    setAllPageProperty("filter", filter, tasks);
}


/* ================ LogFileNormalSaver ================ */
log4qtFileNormalSaver::log4qtFileNormalSaver(QObject *parent) :
    log4qtFileSaverBase(parent)
{
}

log4qtFileSaveTask* log4qtFileNormalSaver::createTask(const QString& category) const
{
    auto task = new log4qtFileNormalSaveTask(property("dir").toString(), category, const_cast<log4qtFileNormalSaver*>(this));
    task->setProperty("flushCount", flushCount);
    connect(this, &log4qtFileNormalSaver::record,
            task, &log4qtFileSaveTask::record,
            Qt::QueuedConnection);
    return task;
}

/* ================ LogFileMmapSaver ================ */
log4qtFileMmapSaver::log4qtFileMmapSaver(QObject *parent) :
    log4qtFileSaverBase(parent)
{
}

log4qtFileSaveTask* log4qtFileMmapSaver::createTask(const QString& category) const
{
    auto task = new log4qtFileMmapSaveTask(property("dir").toString(), category, const_cast<log4qtFileMmapSaver*>(this));
    task->setProperty("mapSize", mapSize);
    connect(this, &log4qtFileMmapSaver::record,
            task, &log4qtFileSaveTask::record,
            Qt::DirectConnection);
    return task;
}
