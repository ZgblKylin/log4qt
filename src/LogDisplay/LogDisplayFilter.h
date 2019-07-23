#pragma once

#include "../Common.h"
#include "LogDisplayModel.h"

namespace log4qt {
class LogDisplayFilter : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QtMsgType typeFilter READ filter WRITE setFilter)
public:
    explicit LogDisplayFilter(LogDisplayModel* model, QObject* parent = nullptr);
    virtual ~LogDisplayFilter() = default;

    QtMsgType filter() const;
    void setFilter(QtMsgType typeFilter);

    bool columnEnabled(int column) const;
    void setColumnEnabled(int column, bool enabled);

    // QSortFilterProxyModel interface
    virtual bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    virtual bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;

private:
    QtMsgType typeFilter = QtWarningMsg;
    QBitArray columnsEnabled;
    LogDisplayModel* model = nullptr;
};
} // namespace log4qt
