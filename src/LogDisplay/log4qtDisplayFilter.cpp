#include "log4qtDisplayFilter.h"

LogDisplayFilter::LogDisplayFilter(LogDisplayModel* m, QObject *parent) :
    QSortFilterProxyModel(parent),
    model(m)
{
    columnsEnabled.resize(model->columnCount());
    columnsEnabled[int(Column::DateTime)] = true;
    columnsEnabled[int(Column::Level)] = true;
    columnsEnabled[int(Column::Thread)] = false;
    columnsEnabled[int(Column::FileLine)] = false;
    columnsEnabled[int(Column::Function)] = false;
    columnsEnabled[int(Column::Message)] = true;
    setSourceModel(m);
}

int LogDisplayFilter::getFilter() const
{
    return filter;
}

void LogDisplayFilter::setFilter(int f)
{
    beginResetModel();
    filter = f;
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
    return level >= filter;
}

bool LogDisplayFilter::filterAcceptsColumn(int source_column, const QModelIndex&) const
{
    return columnsEnabled.at(source_column);
}
