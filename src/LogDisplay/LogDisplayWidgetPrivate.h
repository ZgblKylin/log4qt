#pragma once

#include <mutex>
#include <shared_mutex>
#include <QtCore/QCoreApplication>
#include <QtCore/QThread>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStackedLayout>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <processors/LogDisplayWidget.h>
#include "LogDisplayModel.h"
#include "LogDisplayFilter.h"

namespace log4qt {
class LogDisplayBufferPrivate
{
    Q_DECLARE_PUBLIC(LogDisplayBuffer)
    LogDisplayBuffer* q_ptr;

public:
    LogDisplayBufferPrivate(LogDisplayBuffer* parent) : q_ptr(parent) {}

    std::shared_mutex mutex; // use as spin-lock
    std::vector<LogMessage> messages;
};

class LogDisplaySignalSender : public QObject
{
    Q_OBJECT
public:
    Q_SIGNAL void log(const LogMessage& message);
};

class LogDisplayPagePrivate
{
    Q_DECLARE_PUBLIC(LogDisplayPage)
    LogDisplayPage* q_ptr;

public:
    LogDisplayPagePrivate(LogDisplayPage* parent) : q_ptr(parent) {}
    void log(LogMessage message);

    void onSetDisplayFilter();
    void onSelectColumn();

    LogDisplayModel* dataModel;
    LogDisplayFilter* filterModel;

    LogDisplaySignalSender sender;
    QBoxLayout* widgetLayout;
    QTableView* table;
    QMenu* menu;
    QAction* actionSetDisplayFilter;
    QAction* actionSelectColumn;
    QAction* actionClear;
};

class LogDisplayWidgetPrivate
{
    Q_DECLARE_PUBLIC(LogDisplayWidget)
    LogDisplayWidget* q_ptr;

public:
    LogDisplayWidgetPrivate(LogDisplayWidget* parent) : q_ptr(parent) {}

    LogDisplayPage* createPage(const QString& category);
    void setAllTaskProperty(const char* name, const QVariant& value);

    void log(LogMessage message);

    void objectNameChanged(const QString& objectName);

    // properties
    QtMsgType filter = QtWarningMsg;
    int maxCount = 10000;
    QColor debugForeground = Qt::white;
    QColor informationForeground = Qt::white;
    QColor warningForeground = Qt::white;
    QColor criticalForeground = Qt::white;
    QColor fatalForeground = Qt::white;
    QColor debugBackground = QColor::fromRgb(0x6C7EC2);
    QColor informationBackground = QColor::fromRgb(0x50BC6C);
    QColor warningBackground = QColor::fromRgb(0xDE9E24);
    QColor criticalBackground = QColor::fromRgb(0xD73333);
    QColor fatalBackground = QColor::fromRgb(0x591111);

    LogDisplaySignalSender sender;
    QVBoxLayout* widgetLayout;
    QHBoxLayout* titleLayout;
    QToolButton* leftButton;
    QLabel* title;
    QToolButton* rightButton;
    QStackedLayout* stackedLayout;
};
} // namespace log4qt
