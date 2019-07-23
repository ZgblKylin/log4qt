#include <processors/LogSqlSaver.h>
#include <QtCore/QtCore>
#include <QtSql/QtSql>
#include "../Common.h"

namespace log4qt {
class LogSqlSaverPrivate
{
    Q_DECLARE_PUBLIC(LogSqlSaver)
    LogSqlSaver* q_ptr;

public:
    LogSqlSaverPrivate(LogSqlSaver* parent) : q_ptr(parent) {}

    QSqlDatabase db;

    QString queryString;
    QSqlQuery query;
    QMap<Field, QString> boundValues;

    QTimer timer;
    int transactionTimeout = 1000;

    bool recuse = false;
};

LogSqlSaver::LogSqlSaver(const QSqlDatabase& db, const QString& query, QObject* parent)
    : QObject(parent),
      d_ptr(new LogSqlSaverPrivate(this))
{
    Q_D(LogSqlSaver);

    d->db = db;

    d->queryString = query;
    d->query = QSqlQuery(d->db);
    d->query.prepare(query);

    connect(&d->timer, &QTimer::timeout, [d]{
        d->db.commit();
        d->db.transaction();
    });
    d->timer.setInterval(1000);
    setTransaction(true);
}

LogSqlSaver::~LogSqlSaver()
{
}

QString LogSqlSaver::query() const
{
    Q_D(const LogSqlSaver);
    return d->queryString;
}

void LogSqlSaver::bindValue(const QString& placeholder, Field field)
{
    Q_D(LogSqlSaver);
    d->boundValues.insert(field, placeholder);
}

bool LogSqlSaver::isTransaction() const
{
    Q_D(const LogSqlSaver);
    return d->timer.isActive();
}

void LogSqlSaver::setTransaction(bool enabled)
{
    Q_D(LogSqlSaver);

    if (!enabled)
    {
        d->timer.stop();
        d->db.commit();
    }
    else
    {
        d->db.transaction();
        d->timer.start();
    }
}

int LogSqlSaver::transactionInterval() const
{
    Q_D(const LogSqlSaver);
    return d->timer.interval();
}

void LogSqlSaver::setTransactionInterval(int interval)
{
    Q_D(LogSqlSaver);
    d->timer.setInterval(interval);
}

void LogSqlSaver::log(QtMsgType type, const QMessageLogContext& context, const QString& buf)
{
    Q_D(LogSqlSaver);

    QSqlQuery query(d->query);
    for (auto it = d->boundValues.cbegin(); it != d->boundValues.cend(); ++it)
    {
        switch (it.key())
        {
        case Field::AppName:
            query.bindValue(it.value(), qApp->applicationName());
            break;
        case Field::Category:
            query.bindValue(it.value(), context.category);
            break;
        case Field::DateTime:
            query.bindValue(it.value(), QDateTime::currentDateTime());
            break;
        case Field::Type:
            query.bindValue(it.value(), int(type));
            break;
        case Field::Pid:
            query.bindValue(it.value(), qApp->applicationPid());
            break;
        case Field::ThreadId:
            query.bindValue(it.value(), quintptr(QThread::currentThreadId()));
            break;
        case Field::ThreadPtr:
            query.bindValue(it.value(), quintptr(QThread::currentThread()));
            break;
        case Field::File:
            query.bindValue(it.value(), context.file);
            break;
        case Field::Line:
            query.bindValue(it.value(), context.line);
            break;
        case Field::Function:
            query.bindValue(it.value(), context.function);
            break;
        case Field::Message:
            query.bindValue(it.value(), buf);
            break;
        }
    }

    if(!query.exec() && !(d->recuse))
    {
        d->recuse = true;
        qWarning() << "log query execued failed:"
                   << query.lastQuery()
                   << query.lastError().text();
    }
    d->recuse = false;
}
} // namespace log4qt
