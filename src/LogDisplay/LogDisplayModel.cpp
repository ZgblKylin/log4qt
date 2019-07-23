#include "LogDisplayModel.h"

namespace log4qt {
LogDisplayModel::LogDisplayModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

size_t LogDisplayModel::maxCount() const
{
    return max;
}

void LogDisplayModel::setMaxCount(size_t value)
{
    max = value;
    checkLimit();
}

void LogDisplayModel::clear()
{
    beginResetModel();
    messages.clear();
    endResetModel();
}

void LogDisplayModel::log(LogMessage&& message)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    messages.emplace_back(message);
    endInsertRows();

    checkLimit();
}

int LogDisplayModel::rowCount(const QModelIndex&) const
{
    return int(messages.size());
}

int LogDisplayModel::columnCount(const QModelIndex&) const
{
    return int(Column::ColumnCount);
}

QVariant LogDisplayModel::data(const QModelIndex& index, int role) const
{
    const LogMessage& message = messages.at(size_t(index.row()));

    switch(role)
    {
    case Qt::TextAlignmentRole:
        switch(Column(index.column()))
        {
        case Column::Category:
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
        case Column::Category:
            return message.category;
        case Column::DateTime:
            return message.dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss zzz"));
        case Column::Level:
            return typeToString(message.type);
        case Column::Process:
            return QStringLiteral("%1(%2)")
                    .arg(message.appName)
                    .arg(message.pid);
        case Column::ThreadId:
            return QStringLiteral("%1").arg(quintptr(message.threadId));
        case Column::ThreadPtr:
            return QStringLiteral("@%1")
                    .arg(quintptr(message.threadPtr),
                         sizeof(quintptr) * 2,
                         16,
                         QLatin1Char('0'))
                    .toUpper();
        case Column::FileLine:
            return QStringLiteral("%1:%2")
                    .arg(message.file)
                    .arg(message.line);
        case Column::Function:
            return message.function;
        case Column::Message:
            return message.message;
        default:
            break;
        };
        break;
    case Qt::UserRole:
        switch(Column(index.column()))
        {
        case Column::Category:
            return message.category;
        case Column::DateTime:
            return message.dateTime;
        case Column::Level:
            return message.type;
        case Column::Process:
            return QVariant::fromValue(qMakePair(message.appName, message.pid));
        case Column::ThreadId:
            return quintptr(message.threadId);
        case Column::ThreadPtr:
            return quintptr(message.threadPtr);
        case Column::FileLine:
            return QVariant::fromValue(qMakePair(message.file, message.line));
        case Column::Function:
            return message.function;
        case Column::Message:
            return message.message;
        default:
            break;
        };
        break;
    case Qt::ForegroundRole:
        switch(message.type)
        {
        case QtDebugMsg:
            return debugForeground;
        case QtWarningMsg:
            return warningForeground;
        case QtCriticalMsg:
            return criticalForeground;
        case QtFatalMsg:
            return fatalForeground;
        case QtInfoMsg:
            return informationForeground;
        }
    case Qt::BackgroundRole:
        switch(message.type)
        {
        case QtDebugMsg:
            return debugBackground;
        case QtWarningMsg:
            return warningBackground;
        case QtCriticalMsg:
            return criticalBackground;
        case QtFatalMsg:
            return fatalBackground;
        case QtInfoMsg:
            return informationBackground;
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
        case Column::Category:
            return tr("Category");
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
    if(messages.size() > max)
    {
        int removeCount = int(messages.size() - max);
        beginRemoveRows(QModelIndex(), 0, removeCount - 1);
        messages.erase(messages.begin(), messages.begin() + removeCount);
        endRemoveRows();
    }
}
} // namespace log4qt
