#pragma once

#include <vector>
#include <QtCore/QAbstractTableModel>
#include <QtGui/QColor>
#include <processors/LogDisplayWidget.h>

namespace log4qt {
// Column indexes, correspond to LogMessage
enum class Column
{
    Category = 0,
    DateTime,
    Level,
    Process,
    ThreadId,
    ThreadPtr,
    FileLine,
    Function,
    Message,

    ColumnCount
};

class LogDisplayModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(size_t maxCount READ maxCount WRITE setMaxCount)
    Q_PROPERTY(QColor debugForeground MEMBER debugForeground)
    Q_PROPERTY(QColor warningForeground MEMBER warningForeground)
    Q_PROPERTY(QColor criticalForeground MEMBER criticalForeground)
    Q_PROPERTY(QColor fatalForeground MEMBER fatalForeground)
    Q_PROPERTY(QColor informationForeground MEMBER informationForeground)
    Q_PROPERTY(QColor debugBackground MEMBER debugBackground)
    Q_PROPERTY(QColor warningBackground MEMBER warningBackground)
    Q_PROPERTY(QColor criticalBackground MEMBER criticalBackground)
    Q_PROPERTY(QColor fatalBackground MEMBER fatalBackground)
    Q_PROPERTY(QColor informationBackground MEMBER informationBackground)
public:
    explicit LogDisplayModel(QObject *parent = nullptr);
    virtual ~LogDisplayModel() = default;

    size_t maxCount() const;
    void setMaxCount(size_t value);

    // clear data
    Q_SLOT void clear();

    // Add new log message
    void log(LogMessage&& message);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    // remove log messages exceed maxCount
    void checkLimit();

    size_t max = 10000;
    std::vector<LogMessage> messages;

    /* ================ color settings ================ */
    QColor debugForeground = Qt::white;
    QColor warningForeground = Qt::white;
    QColor criticalForeground = Qt::white;
    QColor fatalForeground = Qt::white;
    QColor informationForeground = Qt::white;
    QColor debugBackground = QColor::fromRgb(0x6C7EC2);
    QColor warningBackground = QColor::fromRgb(0xDE9E24);
    QColor criticalBackground = QColor::fromRgb(0xD73333);
    QColor fatalBackground = QColor::fromRgb(0x591111);
    QColor informationBackground = QColor::fromRgb(0x50BC6C);
};
} // namespace log4qt
