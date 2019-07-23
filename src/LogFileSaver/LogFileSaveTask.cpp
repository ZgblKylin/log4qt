#include "LogFileSaveTask.h"

namespace log4qt {
/* ================ LogFileSaveTask ================ */
LogFileSaveTask::LogFileSaveTask(const QDir& defaultDir, const QString& categoryName, QObject* parent)
    : file(new QFile(this)),
      category(categoryName),
      thread(new QThread)
{
    setDir(defaultDir.absoluteFilePath(categoryName));

    moveToThread(thread);
    connect(parent, &QObject::destroyed,
            this, [=]{ delete this; },
            Qt::BlockingQueuedConnection);
    connect(this, &QObject::destroyed,
            thread, &QThread::quit);
    connect(thread, &QThread::finished,
            thread, &QObject::deleteLater);
    thread->start(QThread::LowPriority);
}

LogFileSaveTask::~LogFileSaveTask()
{
    while(!mutex.tryLock()) {}
    defer [this]{ mutex.unlock(); };
    closeFile();
    thread->quit();
    thread->deleteLater();
}

void LogFileSaveTask::record(const QSharedPointer<LogMessage> message)
{
    if(message->category != category) return;
    if(message->type < filter) return;

    QString text = log4qt::formatMessage(pattern, *message);
    QByteArray bytes;
    QTextStream ts(&bytes, QIODevice::ReadWrite | QIODevice::Text);
    ts.setCodec(codec);
    ts << text << endl;
    ts.flush();

    while(!mutex.tryLock()) {}
    defer [this]{ mutex.unlock(); };
    refreshFile();
    save(bytes);
}

QString LogFileSaveTask::dir() const
{
    return dr.absolutePath();
}

void LogFileSaveTask::setDir(const QString& path)
{
    while(!mutex.tryLock()) {}
    defer [this]{ mutex.unlock(); };
    dr.setPath(path);
    if(!dr.exists()) dr.mkpath(dr.absolutePath());
    closeFile();
}

void LogFileSaveTask::refreshFile()
{
    if(Q_UNLIKELY(!file)) file = new QFile(this);
    if(Q_LIKELY(file->isOpen() && (file->size() <= maxFileSize))) return;

    closeFile();

    // file name is datetime
    QString newName = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss"));
    if(Q_UNLIKELY(fileName == newName)) {
        ++fileIndex;
    } else {
        fileIndex = 1;
    }
    fileName = newName;

    // add suffix if file duplicated
    if(Q_LIKELY(fileIndex == 1))
    {
        file->setFileName(dr.absoluteFilePath(QStringLiteral("%1.log"))
                          .arg(fileName));
    }
    else
    {
        file->setFileName(dr.absoluteFilePath(QStringLiteral("%1_%2.log"))
                          .arg(fileName).arg(fileIndex));
    }

    file->open(QFile::ReadWrite | QFile::Text | QFile::Truncate);
}

void LogFileSaveTask::closeFile()
{
    if(file->isOpen())
    {
        file->close();
        if(file->size() == 0) file->remove();
    }
}
/* ================ LogFileSaveTask ================ */

/* ================ LogFileNormalSaveTask ================ */
LogFileNormalSaveTask::LogFileNormalSaveTask(const QDir& defaultDir, const QString& category, QObject* parent)
    : LogFileSaveTask(defaultDir, category, parent)
{
}

void LogFileNormalSaveTask::save(const QByteArray& text)
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
/* ================ LogFileNormalSaveTask ================ */

/* ================ LogFileMmapSaveTask ================ */
LogFileMmapSaveTask::LogFileMmapSaveTask(const QDir& defaultDir, const QString& category, QObject* parent)
    : LogFileSaveTask(defaultDir, category, parent)
{
}

void LogFileMmapSaveTask::save(const QByteArray& text)
{
    // p is writing position
    for(const char* p = text.cbegin(); p != text.cend();)
    {
        // refresh mmap block
        if(availableSize == 0) map();

        // remain size to be written
        const qint64 remain = text.cend() - p;
        // available size to write
        const qint64 writeSize = std::min(remain, availableSize);

        if(Q_LIKELY(block))
        {
            memcpy(block, p, size_t(writeSize));
            block += writeSize;
            availableSize -= writeSize;
        }
        p += writeSize;
    }
}

void LogFileMmapSaveTask::map()
{
    // expand file size to next block
    file->resize(file->size() + mapSize);

    // unmap last block
    if(Q_LIKELY(block)) file->unmap(block - mapSize);

    // mmap new block
    block = file->map(mapPos, mapSize);
    if(Q_LIKELY(block))
    {
        mapPos += mapSize;
        availableSize = mapSize;
        memset(block, '\0', size_t(availableSize));
    }
}
/* ================ LogFileMmapSaveTask ================ */
} // namespace log4qt
