#include <QtSql/QSqlError>
#include <processors/log4qtSqlSaver.h>

log4qtSqlSaver::log4qtSqlSaver(QSqlDatabase database, QObject* parent)
    : log4qt::impl::ILogProcessor(parent),
      db(database),
      query(database)
{
    query.prepare(QStringLiteral("INSERT INTO Log (time, level, threadid, threadptr, file, line, function, message) "
                                 "VALUES (:%1, :%2, :%3, :%4, :%5, :%6, :%7, :%8)")
                  .arg(log4qt::impl::PatternDateTime).arg(log4qt::impl::PatternLevel)
                  .arg(log4qt::impl::PatternPID).arg(log4qt::impl::PatternThreadId)
                  .arg(log4qt::impl::PatternThreadPtr).arg(log4qt::impl::PatternFile)
                  .arg(log4qt::impl::PatternLine).arg(log4qt::impl::PatternFunction)
                  .arg(log4qt::impl::PatternMessage));

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
    q.bindValue(2, QString::number(message->pid));
    q.bindValue(3, QStringLiteral("%1").arg(quintptr(message->threadid), sizeof(quintptr) * 2, 16, QLatin1Char('0')));
    q.bindValue(4, QStringLiteral("%1").arg(quintptr(message->threadptr), sizeof(quintptr) * 2, 16, QLatin1Char('0')));
    q.bindValue(5, message->file);
    q.bindValue(6, message->line);
    q.bindValue(7, message->function);
    q.bindValue(8, *(message->message));
    if(!q.exec())
    {
        qDebug() << q.lastQuery()
                 << q.lastError().text();
    }
}
