#include <QtGui/QFontMetrics>
#include <QtGui/QColor>
#include <QtWidgets/QWidget>
#include "log4qtDisplayModel.h"


LogDisplayModel::LogDisplayModel(QObject* parent) :
    QAbstractTableModel(parent)
{
}

int LogDisplayModel::getMaxCount() const
{
    return maxCount;
}

void LogDisplayModel::setMaxCount(int value)
{
    maxCount = value;
    checkLimit();
}

void LogDisplayModel::clear()
{
    beginResetModel();
    messages.clear();
    endResetModel();
}

void LogDisplayModel::newLog(QSharedPointer<log4qt::impl::LogMessage> message)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    messages << message;
    endInsertRows();

    checkLimit();
}

int LogDisplayModel::rowCount(const QModelIndex&) const
{
    return messages.count();
}

int LogDisplayModel::columnCount(const QModelIndex&) const
{
    return int(Column::ColumnCount);
}

QVariant LogDisplayModel::data(const QModelIndex& index, int role) const
{
    QSharedPointer<log4qt::impl::LogMessage> message = messages.at(index.row());

    switch(role)
    {
    case Qt::TextAlignmentRole:
        switch(Column(index.column()))
        {
        case Column::DateTime:
        case Column::Level:
        case Column::Process:
        case Column::ThreadId:
        case Column::ThreadPtr:
            return Qt::AlignCenter;
        default:
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        }
    case Qt::DisplayRole:
    case Qt::AccessibleTextRole:
        switch(Column(index.column()))
        {
        case Column::DateTime:
            return message->dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss zzz"));
        case Column::Level:
            return QStringLiteral("%1(%2)")
                    .arg(log4qt::impl::getLogTypeString(message->type)).arg(message->level);
        case Column::Process:
            return message->pid;
        case Column::ThreadId:
            return QStringLiteral("@%1")
                    .arg(quintptr(message->threadid), sizeof(quintptr) * 2, 16, QLatin1Char('0'))
                    .toUpper();
        case Column::ThreadPtr:
            return QStringLiteral("@%1")
                    .arg(quintptr(message->threadptr), sizeof(quintptr) * 2, 16, QLatin1Char('0'))
                    .toUpper();
        case Column::FileLine:
            return QStringLiteral("%1:%2")
                    .arg(message->file)
                    .arg(message->line);
        case Column::Function:
            return message->function;
        case Column::Message:
            return *(message->message);
        default:
            break;
        };
        break;
    case Qt::UserRole:
        switch(Column(index.column()))
        {
        case Column::DateTime:
            return message->dateTime;
        case Column::Level:
            return message->level;
        case Column::Process:
            return message->pid;
        case Column::ThreadId:
            return quintptr(message->threadid);
        case Column::ThreadPtr:
            return quintptr(message->threadptr);
        case Column::FileLine:
            return QVariant::fromValue(qMakePair(message->file, message->line));
        case Column::Function:
            return message->function;
        case Column::Message:
            return *(message->message);
        default:
            break;
        };
        break;
    case Qt::ForegroundRole:
        switch(message->type)
        {
        case log4qt::Debug:
            return debugForeground;
        case log4qt::Infomation:
            return infomationForeground;
        case log4qt::Warning:
            return warningForeground;
        case log4qt::Critical:
            return criticalForeground;
        case log4qt::Fatal:
            return fatalForeground;
        }
    case Qt::BackgroundRole:
        switch(message->type)
        {
        case log4qt::Debug:
            return debugBackground;
        case log4qt::Infomation:
            return infomationBackground;
        case log4qt::Warning:
            return warningBackground;
        case log4qt::Critical:
            return criticalBackground;
        case log4qt::Fatal:
            return fatalBackground;
        }
    default:
        return QVariant();
    }

    return QVariant();
}

QVariant LogDisplayModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return QAbstractTableModel::headerData(section, orientation, role);

    switch(role)
    {
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::AccessibleTextRole:
        switch(Column(section))
        {
        case Column::DateTime:
            return tr("DateTime");
        case Column::Level:
            return tr("Level");
        case Column::Process:
            return tr("Process");
        case Column::ThreadId:
            return tr("ThreadId");
        case Column::ThreadPtr:
            return tr("ThreadPtr");
        case Column::FileLine:
            return tr("FileLine");
        case Column::Function:
            return tr("Function");
        case Column::Message:
            return tr("Message");
        default:
            break;
        }
    default:
        break;
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

void LogDisplayModel::checkLimit()
{
    if(messages.count() > maxCount)
    {
        int removeCount = messages.count() - maxCount;
        beginRemoveRows(QModelIndex(), 0, removeCount - 1);
        messages.erase(messages.begin(), messages.begin() + removeCount);
        endRemoveRows();
    }
}
