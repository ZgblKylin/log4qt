#ifndef LOG4QTDISPLAYFILTER_H
#define LOG4QTDISPLAYFILTER_H

#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QBitArray>
#include <log4qt.h>
#include "log4qtDisplayModel.h"


class log4qtDisplayFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int filter READ getFilter WRITE setFilter)
public:
    explicit log4qtDisplayFilter(log4qtDisplayModel* model, QObject *parent = 0);
    virtual ~log4qtDisplayFilter() = default;

    int getFilter() const;
    void setFilter(int filter);

    bool columnEnabled(int column) const;
    void setColumnEnabled(int column, bool enabled);

    // QSortFilterProxyModel interface
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    virtual bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;

private:
    int filter = log4qt::impl::DefaultFilter;
    QBitArray columnsEnabled;
    log4qtDisplayModel* model = nullptr;
};

#endif // LOG4QTDISPLAYFILTER_H
