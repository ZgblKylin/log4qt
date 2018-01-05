#ifndef LOG4QTDISPLAYPAGE_H
#define LOG4QTDISPLAYPAGE_H

#include <QtCore/QQueue>
#include <QtGui/QColor>
#include <QtWidgets/QTableView>
#include <log4qt.h>

class LogDisplayModel;
class LogDisplayFilter;
class QMenu;

// widget for displaying log message of single category
class log4qtDisplayPage : public QTableView
{
    Q_OBJECT
    // message with level under filter will not be processed, default is Infomation
    Q_PROPERTY(int getFilter READ filter WRITE setFilter)
    // max count to be shown(before filter), default is 10000
    Q_PROPERTY(int getMaxCount READ maxCount WRITE setMaxCount)
    // foreground color for log text
    Q_PROPERTY(QColor debugForeground READ debugForeground WRITE setDebugForeground)
    Q_PROPERTY(QColor infomationForeground READ infomationForeground WRITE setInfomationForeground)
    Q_PROPERTY(QColor warningForeground READ warningForeground WRITE setWarningForeground)
    Q_PROPERTY(QColor criticalForeground READ criticalForeground WRITE setCriticalForeground)
    Q_PROPERTY(QColor fatalForeground READ fatalForeground WRITE setFatalForeground)
    // background color for log text
    Q_PROPERTY(QColor debugBackground READ debugBackground WRITE setDebugBackground)
    Q_PROPERTY(QColor infomationBackground READ infomationBackground WRITE setInfomationBackground)
    Q_PROPERTY(QColor warningBackground READ warningBackground WRITE setWarningBackground)
    Q_PROPERTY(QColor criticalBackground READ criticalBackground WRITE setCriticalBackground)
    Q_PROPERTY(QColor fatalBackground READ fatalBackground WRITE setFatalBackground)

public:
    explicit log4qtDisplayPage(QWidget* parent = 0);
    virtual ~log4qtDisplayPage() = default;

    void log(const QSharedPointer<log4qt::impl::LogMessage> message);

private:
    // message with level under filter will not be processed
    Q_SLOT void on_setDisplayFilter_triggered();

    // select which columns will be shown
    Q_SLOT void on_selectColumn_triggered();

    int filter() const;
    void setFilter(int value);

    int maxCount() const;
    void setMaxCount(int value);

    QColor debugForeground() const;
    void setDebugForeground(QColor color);
    QColor infomationForeground() const;
    void setInfomationForeground(QColor color);
    QColor warningForeground() const;
    void setWarningForeground(QColor color);
    QColor criticalForeground() const;
    void setCriticalForeground(QColor color);
    QColor fatalForeground() const;
    void setFatalForeground(QColor color);

    QColor debugBackground() const;
    void setDebugBackground(QColor color);
    QColor infomationBackground() const;
    void setInfomationBackground(QColor color);
    QColor warningBackground() const;
    void setWarningBackground(QColor color);
    QColor criticalBackground() const;
    void setCriticalBackground(QColor color);
    QColor fatalBackground() const;
    void setFatalBackground(QColor color);

    LogDisplayModel* model;
    LogDisplayFilter* filterModel;
    QMenu* menu;
};

#endif // LOG4QTDISPLAYPAGE_H
