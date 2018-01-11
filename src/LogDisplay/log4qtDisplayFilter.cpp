#include "log4qtDisplayFilter.h"

log4qtDisplayFilter::log4qtDisplayFilter(log4qtDisplayModel* m, QObject *parent) :
    QSortFilterProxyModel(parent),
    model(m)
{
    columnsEnabled.resize(model->columnCount());
    columnsEnabled[int(Column::DateTime)] = true;
    columnsEnabled[int(Column::Level)] = true;
    columnsEnabled[int(Column::ThreadId)] = false;
    columnsEnabled[int(Column::FileLine)] = false;
    columnsEnabled[int(Column::Function)] = false;
    columnsEnabled[int(Column::Message)] = true;
    setSourceModel(m);
}

int log4qtDisplayFilter::getFilter() const
{
    return filter;
}

void log4qtDisplayFilter::setFilter(int f)
{
    beginResetModel();
    filter = f;
    endResetModel();
}

bool log4qtDisplayFilter::columnEnabled(int column) const
{
    return columnsEnabled.at(column);
}

void log4qtDisplayFilter::setColumnEnabled(int column, bool enabled)
{
    beginResetModel();
    columnsEnabled[column] = enabled;
    endResetModel();
}

bool log4qtDisplayFilter::filterAcceptsRow(int source_row, const QModelIndex&) const
{
    int level = model->index(source_row, int(Column::Level)).data(Qt::UserRole).toInt();
    return level >= filter;
}

bool log4qtDisplayFilter::filterAcceptsColumn(int source_column, const QModelIndex&) const
{
    return columnsEnabled.at(source_column);
}
