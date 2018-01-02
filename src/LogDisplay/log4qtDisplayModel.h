#ifndef LOGDISPLAYMODEL_H
#define LOGDISPLAYMODEL_H

#include <QtCore/QDateTime>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QSize>
#include <QtGui/QColor>
#include <vector>
#include <deque>
#include <log4qt.h>


// Column indexes, correspond to LogMessage
enum class Column
{
    DateTime = 0,
    Level,
    Process,
    Thread,
    FileLine,
    Function,
    Message,

    ColumnCount
};

class LogDisplayModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(int maxCount READ getMaxCount WRITE setMaxCount)
    Q_PROPERTY(QColor debugForeground MEMBER debugForeground)
    Q_PROPERTY(QColor infomationForeground MEMBER infomationForeground)
    Q_PROPERTY(QColor warningForeground MEMBER warningForeground)
    Q_PROPERTY(QColor criticalForeground MEMBER criticalForeground)
    Q_PROPERTY(QColor fatalForeground MEMBER fatalForeground)
    Q_PROPERTY(QColor debugBackground MEMBER debugBackground)
    Q_PROPERTY(QColor infomationBackground MEMBER infomationBackground)
    Q_PROPERTY(QColor warningBackground MEMBER warningBackground)
    Q_PROPERTY(QColor criticalBackground MEMBER criticalBackground)
    Q_PROPERTY(QColor fatalBackground MEMBER fatalBackground)
public:
    explicit LogDisplayModel(QObject *parent = 0);
    virtual ~LogDisplayModel() = default;

    int getMaxCount() const;
    void setMaxCount(int value);

    // clear data
    Q_SLOT void clear();

    // add new log message
    void newLog(QSharedPointer<log4qt::impl::LogMessage> message);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    // remove log messages exceed maxCount
    void checkLimit();

    int maxCount = 10000;
    QVector<QSharedPointer<log4qt::impl::LogMessage>> messages;

    /* ================ color settings ================ */
    QColor debugForeground = Qt::white;
    QColor infomationForeground = Qt::white;
    QColor warningForeground = Qt::white;
    QColor criticalForeground = Qt::white;
    QColor fatalForeground = Qt::white;
    QColor debugBackground = QColor::fromRgb(0x6C7EC2);
    QColor infomationBackground = QColor::fromRgb(0x50BC6C);
    QColor warningBackground = QColor::fromRgb(0xDE9E24);
    QColor criticalBackground = QColor::fromRgb(0xD73333);
    QColor fatalBackground = QColor::fromRgb(0x591111);
};

#endif // LOGDISPLAYMODEL_H
