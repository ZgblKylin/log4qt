#include <QCoreApplication>
#include <processors/log4qtFileSaver.h>

/* ================ LogFileSaverBase ================ */
log4qtFileSaverBase::log4qtFileSaverBase(QObject* parent)
    : log4qt::impl::ILogProcessor(parent),
      mutex(QMutex::Recursive)
{
    dir = QDir(qApp->applicationDirPath()
               + QDir::separator()
               + QStringLiteral("Log"));
    if(!dir.exists())
        dir.mkpath(dir.absolutePath());

    codec = QTextCodec::codecForName("UTF-8");

    file = new QFile(this);

    maxFileSize = 100 * 1024 * 1024;

    moveToThread(&thread);
}

log4qtFileSaverBase::~log4qtFileSaverBase()
{
    closeFile();
}

void log4qtFileSaverBase::refreshFile()
{
    QMutexLocker locker(&mutex);

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

void log4qtFileSaverBase::closeFile()
{
    QMutexLocker locker(&mutex);

    if(file->isOpen())
    {
        file->close();
        if(file->size() == 0)
            file->remove();
    }
}

/* ================ LogFileNormalSaver ================ */
log4qtFileNormalSaver::log4qtFileNormalSaver(QObject *parent) :
    log4qtFileSaverBase(parent)
{
    log4qt::impl::LogEngine::registerProcessor(this, Qt::QueuedConnection);
}

void log4qtFileNormalSaver::log(const QSharedPointer<log4qt::impl::LogMessage> message)
{
    if(message->level < filter)
        return;
    QMutexLocker locker(&mutex);

    QString text = log4qt::impl::processMessage(pattern, message);
    QByteArray bytes;
    QTextStream ts(&bytes, QIODevice::ReadWrite | QIODevice::Text);
    ts.setCodec(codec);
    ts << text;
    ts.flush();
    file->write(bytes);

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

/* ================ LogFileMmapSaver ================ */
log4qtFileMmapSaver::log4qtFileMmapSaver(QObject *parent) :
    log4qtFileSaverBase(parent)
{
    log4qt::impl::LogEngine::registerProcessor(this, Qt::DirectConnection);
}

void log4qtFileMmapSaver::log(const QSharedPointer<log4qt::impl::LogMessage> message)
{
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

    // p is writing point
    for(const char* p=bytes.cbegin();p!=bytes.cend();)
    {
        // refresh mmap block
        if(availableSize == 0)
            map();

        // remain size to be written
        const qint64 remain = bytes.cend() - p;
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

void log4qtFileMmapSaver::map()
{
    QMutexLocker locker(&mutex);

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
