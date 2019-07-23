#pragma once

#include <QtCore/QTimer>
#include <QtSql/QSqlQuery>
#include "../log4qt.h"

namespace log4qt {
class LogSqlSaverPrivate;

// Record log into SQL table, using Qt::DirectConnection
class LOG4QTSHARED_EXPORT LogSqlSaver : public QObject, public log4qt::ILogProcessor
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LogSqlSaver)
public:
    // QSqlQuery bound to connected database
    Q_PROPERTY(QString query READ query CONSTANT)
    // Whether use SQL transaction to speed-up recording, default true
    Q_PROPERTY(bool transaction READ isTransaction WRITE setTransaction)
    // transaction commit interval, default is 1000ms
    Q_PROPERTY(int transactionInterval READ transactionInterval WRITE setTransactionInterval)

    LogSqlSaver(const QSqlDatabase& db, const QString& query, QObject* parent = nullptr);
    virtual ~LogSqlSaver() override final;

    QString query() const;
    void bindValue(const QString& placeholder, log4qt::Field field);

    bool isTransaction() const;
    void setTransaction(bool enabled);

    int transactionInterval() const;
    void setTransactionInterval(int interval);

protected:
    // ILogProcessor interface
    virtual void log(QtMsgType type, const QMessageLogContext& context, const QString& buf) override;

    LogSqlSaverPrivate* d_ptr;
};
} // namespace log4qt
