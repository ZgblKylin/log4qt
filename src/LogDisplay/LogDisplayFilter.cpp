#include "LogDisplayFilter.h"

namespace log4qt {
LogDisplayFilter::LogDisplayFilter(LogDisplayModel* m, QObject* parent) :
    QSortFilterProxyModel(parent),
    model(m)
{
    columnsEnabled.resize(model->columnCount());
    columnsEnabled[int(Column::Category)] = false;
    columnsEnabled[int(Column::DateTime)] = true;
    columnsEnabled[int(Column::Level)] = true;
    columnsEnabled[int(Column::Process)] = true;
    columnsEnabled[int(Column::ThreadId)] = false;
    columnsEnabled[int(Column::ThreadPtr)] = false;
    columnsEnabled[int(Column::FileLine)] = false;
    columnsEnabled[int(Column::Function)] = false;
    columnsEnabled[int(Column::Message)] = true;
    setSourceModel(m);
}

QtMsgType LogDisplayFilter::filter() const
{
    return typeFilter;
}

void LogDisplayFilter::setFilter(QtMsgType f)
{
    beginResetModel();
    typeFilter = f;
    endResetModel();
}

bool LogDisplayFilter::columnEnabled(int column) const
{
    return columnsEnabled.at(column);
}

void LogDisplayFilter::setColumnEnabled(int column, bool enabled)
{
    beginResetModel();
    columnsEnabled[column] = enabled;
    endResetModel();
}

bool LogDisplayFilter::filterAcceptsRow(int source_row, const QModelIndex&) const
{
    int level = model->index(source_row, int(Column::Level)).data(Qt::UserRole).toInt();
    return level >= typeFilter;
}

bool LogDisplayFilter::filterAcceptsColumn(int source_column, const QModelIndex&) const
{
    return columnsEnabled.at(source_column);
}
} // namespace log4qt
