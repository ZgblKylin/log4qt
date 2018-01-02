#include <QtSql/QSqlError>
#include <processors/log4qtSqlSaver.h>

log4qtSqlSaver::log4qtSqlSaver(QSqlDatabase database, QObject* parent)
    : log4qt::impl::ILogProcessor(parent),
      db(database),
      query(database)
{
    query.prepare(QStringLiteral("INSERT INTO Log (time, level, threadid, threadptr, file, line, function, message) "
                                 "VALUES (:%1, :%2, :%3, :%4, :%5, :%6, :%7, :%8)")
                  .arg(log4qt::impl::DateTime).arg(log4qt::impl::Level)
                  .arg(log4qt::impl::ThreadId).arg(log4qt::impl::ThreadPtr)
                  .arg(log4qt::impl::File).arg(log4qt::impl::Line)
                  .arg(log4qt::impl::Function).arg(log4qt::impl::Message));

    connect(&timer, &QTimer::timeout, [=]{
        db.commit();
        db.transaction();
    });
    db.transaction();
    timer.start(1000);

    log4qt::impl::LogEngine::registerProcessor(this, Qt::DirectConnection);
}

void log4qtSqlSaver::log(const QSharedPointer<log4qt::impl::LogMessage> message)
{
    QSqlQuery q(query);
    q.bindValue(0, message->dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss zzz")));
    q.bindValue(1, message->level);
    q.bindValue(2, QStringLiteral("%1").arg(quintptr(message->threadid), sizeof(quintptr) * 2, 16, QLatin1Char('0')));
    q.bindValue(3, QStringLiteral("%1").arg(quintptr(message->threadptr), sizeof(quintptr) * 2, 16, QLatin1Char('0')));
    q.bindValue(4, message->file);
    q.bindValue(5, message->line);
    q.bindValue(6, message->function);
    q.bindValue(7, *(message->message));
    if(!q.exec())
    {
        qDebug() << q.lastQuery()
                 << q.lastError().text();
    }
}
