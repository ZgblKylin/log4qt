#ifndef LOG4QTSQLSAVER_H
#define LOG4QTSQLSAVER_H

#include <log4qt.h>
#include <QtCore/QTimer>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class QSqlQuery;
class LOG4QTSHARED_EXPORT log4qtSqlSaver : public log4qt::impl::ILogProcessor
{
    Q_OBJECT
public:
     explicit log4qtSqlSaver(QSqlDatabase database, QObject* parent = 0);
     virtual ~log4qtSqlSaver() = default;

private:
    // ILogProcessor interface
    Q_SLOT virtual void log(const QSharedPointer<log4qt::impl::LogMessage> message) override;

    QSqlDatabase db;
    QSqlQuery query;
    QTimer timer;
};

#endif // LOG4QTSQLSAVER_H
