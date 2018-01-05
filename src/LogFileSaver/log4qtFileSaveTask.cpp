#include "log4qtFileSaveTask.h"

/* ================ log4qtFileSaveTask ================ */
log4qtFileSaveTask::log4qtFileSaveTask(const QDir& defaultDir, const QString& categoryName, QObject* parent)
    : file(new QFile(this)),
      mutex(QMutex::Recursive),
      category(categoryName)
{
    setDir(defaultDir.absoluteFilePath(categoryName));

    pattern = log4qt::impl::parsePattern(pattern);

    moveToThread(&thread);
    connect(parent, &QObject::destroyed,
            this, &QObject::deleteLater);
    thread.start(QThread::LowPriority);
}

log4qtFileSaveTask::~log4qtFileSaveTask()
{
    closeFile();
    thread.quit();
    thread.wait();
}

void log4qtFileSaveTask::record(const QSharedPointer<log4qt::impl::LogMessage> message)
{
    if(message->category != category)
        return;
    if(message->level < filter)
        return;

    QMutexLocker locker(&mutex);

    QString text = log4qt::impl::processMessage(pattern, message);
    QByteArray bytes;
    QTextStream ts(&bytes, QIODevice::ReadWrite | QIODevice::Text);
    ts.setCodec(codec);
    ts << text;
    ts.flush();

    refreshFile();
    save(bytes);
}

QString log4qtFileSaveTask::getDir() const
{
    return dir.absolutePath();
}

void log4qtFileSaveTask::setDir(const QString& path)
{
    QMutexLocker locker(&mutex);
    dir.setPath(path);
    if(!dir.exists())
        dir.mkpath(dir.absolutePath());
}

void log4qtFileSaveTask::refreshFile()
{
    QMutexLocker locker(&mutex);

    if(Q_UNLIKELY(!file))
        file = new QFile(this);
    if(Q_LIKELY(file->isOpen() && (file->size() <= maxFileSize)))
        return;

    closeFile();

    // file name is datetime
    QString newName = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss"));
    if(Q_UNLIKELY(fileName == newName))
        ++fileIndex;
    else
        fileIndex = 1;
    fileName = newName;

    // add suffix if file duplicated
    if(Q_LIKELY(fileIndex == 1))
        file->setFileName(dir.absoluteFilePath(QStringLiteral("%1.log")).arg(fileName));
    else
        file->setFileName(dir.absoluteFilePath(QStringLiteral("%1_%2.log")).arg(fileName).arg(fileIndex));

    file->open(QFile::ReadWrite | QFile::Text | QFile::Truncate);
}

void log4qtFileSaveTask::closeFile()
{
    QMutexLocker locker(&mutex);
    if(file->isOpen())
    {
        file->close();
        if(file->size() == 0)
            file->remove();
    }
}


/* ================ log4qtFileNormalSaveTask ================ */
log4qtFileNormalSaveTask::log4qtFileNormalSaveTask(const QDir& defaultDir, const QString& category, QObject* parent)
    : log4qtFileSaveTask(defaultDir, category, parent)
{
}

void log4qtFileNormalSaveTask::save(const QByteArray& text)
{
    file->write(text);

    ++count;
    if(flushCount >= 0)
    {
        if(count >= flushCount)
        {
            file->flush();
            count = 0;
        }
    }
}


/* ================ log4qtFileMmapSaveTask ================ */
log4qtFileMmapSaveTask::log4qtFileMmapSaveTask(const QDir& defaultDir, const QString& category, QObject* parent)
    : log4qtFileSaveTask(defaultDir, category, parent)
{
}

void log4qtFileMmapSaveTask::save(const QByteArray& text)
{
    // p is writing position
    for(const char* p=text.cbegin();p!=text.cend();)
    {
        // refresh mmap block
        if(availableSize == 0)
            map();

        // remain size to be written
        const qint64 remain = text.cend() - p;
        // available size to write
        const qint64 writeSize = std::min(remain, availableSize);

        if(Q_LIKELY(block))
        {
            memcpy(block, p, writeSize);
            block += writeSize;
            availableSize -= writeSize;
        }
        p += writeSize;
    }
}

void log4qtFileMmapSaveTask::map()
{
    // expand file size to next block
    file->resize(file->size() + mapSize);

    // unmap last block
    if(Q_LIKELY(block))
        file->unmap(block - mapSize);

    // mmap new block
    block = file->map(mapPos, mapSize);
    if(Q_LIKELY(block))
    {
        mapPos += mapSize;
        availableSize = mapSize;
        memset(block, '\0', availableSize);
    }
}
