#ifndef LOG4QTSQLSAVER_H
#define LOG4QTSQLSAVER_H

#include <log4qt.h>
#include <QtCore/QTimer>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

// record log into SQL table, using transaction to flush message per sec
class LOG4QTSHARED_EXPORT log4qtSqlSaver : public log4qt::impl::ILogProcessor
{
    Q_OBJECT
public:
    explicit log4qtSqlSaver(QSqlDatabase database, const QString& tableName = "Log", QObject* parent = 0);
    virtual ~log4qtSqlSaver() = default;

    // return SQL query to create log table using tableName
    static QString createTableSql(const QString& tableName);

    // ILogProcessor interface
    virtual void start() override;
    Q_SLOT virtual void log(const QSharedPointer<log4qt::impl::LogMessage> message) override;

private:
    QSqlDatabase db;
    QSqlQuery query;
    QTimer timer;
};

#endif // LOG4QTSQLSAVER_H
