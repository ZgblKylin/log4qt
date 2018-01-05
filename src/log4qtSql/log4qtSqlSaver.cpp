#include <QtSql/QSqlError>
#include <processors/log4qtSqlSaver.h>


log4qtSqlSaver::log4qtSqlSaver(QSqlDatabase database, const QString& tableName, QObject* parent)
    : log4qt::impl::ILogProcessor(parent),
      db(database),
      query(database)
{
    query.prepare(QStringLiteral("INSERT INTO %1 (category, time, level, pid, threadid, threadptr, file, line, function, message) "
                                 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)")
                  .arg(tableName));

    connect(&timer, &QTimer::timeout, [=]{
        db.commit();
        db.transaction();
    });
    db.transaction();
    timer.start(1000);
}

QString log4qtSqlSaver::createTableSql(const QString& tableName)
{
    return QStringLiteral("CREATE TABLE [%1]("
                          "[category] TEXT, "
                          "[time] DATETIME NOT NULL, "
                          "[level] TINYINT NOT NULL, "
                          "[pid] BIGINT NOT NULL, "
                          "[threadid] BIGINT NOT NULL, "
                          "[threadptr] BIGINT NOT NULL, "
                          "[file] TEXT NOT NULL, "
                          "[line] INT NOT NULL, "
                          "[function] TEXT NOT NULL, "
                          "[message] TEXT NOT NULL);")
            .arg(tableName);
}

void log4qtSqlSaver::start()
{
    log4qt::impl::getLogEngine()->registerProcessor(this, Qt::DirectConnection);
}

void log4qtSqlSaver::log(const QSharedPointer<log4qt::impl::LogMessage> message)
{
    QSqlQuery q(query);
    q.bindValue(0, message->category);
    q.bindValue(1, message->dateTime);
    q.bindValue(2, message->level);
    q.bindValue(3, QString::number(message->pid));
    q.bindValue(4, quintptr(message->threadid));
    q.bindValue(5, quintptr(message->threadptr));
    q.bindValue(6, message->file);
    q.bindValue(7, message->line);
    q.bindValue(8, message->function);
    q.bindValue(9, *(message->message));
    if(!q.exec())
    {
        qDebug() << q.lastQuery()
                 << q.lastError().text();
    }
}
