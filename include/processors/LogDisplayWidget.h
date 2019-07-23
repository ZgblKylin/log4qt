#pragma once

#include <vector>
#include <QtCore/QtGlobal>
#include <QtGui/QColor>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTableView>
#include "../log4qt.h"

class QStackedLayout;

namespace log4qt {
class LogDisplayBufferPrivate;
class LogDisplayPagePrivate;
class LogDisplayWidgetPrivate;
class LogDisplayWidget;

// buffer for caching log messages before log4qtDisplayWidget constructed
class LOG4QTSHARED_EXPORT LogDisplayBuffer : public QObject, public ILogProcessor
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LogDisplayBuffer)
public:
    explicit LogDisplayBuffer(QObject* parent = nullptr);
    virtual ~LogDisplayBuffer() override;

    // ILogProcessor interface
    virtual void log(QtMsgType type, const QMessageLogContext& context, const QString& buf) override;

    // take all stored log messages, stop recording log
    std::vector<LogMessage> takeAll();

private:
    LogDisplayBufferPrivate* d_ptr;
};

// widget for displaying log message of single category
class LOG4QTSHARED_EXPORT LogDisplayPage : public QWidget, public ILogProcessor
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LogDisplayPage)
    // message with level under filter will not be processed, default is Information
    Q_PROPERTY(QtMsgType filter READ filter WRITE setFilter)
    // max count to be shown(before filter), default is 10000
    Q_PROPERTY(size_t maxCount READ maxCount WRITE setMaxCount)
    // foreground color for log text
    Q_PROPERTY(QColor debugForeground READ debugForeground WRITE setDebugForeground)
    Q_PROPERTY(QColor warningForeground READ warningForeground WRITE setWarningForeground)
    Q_PROPERTY(QColor criticalForeground READ criticalForeground WRITE setCriticalForeground)
    Q_PROPERTY(QColor fatalForeground READ fatalForeground WRITE setFatalForeground)
    Q_PROPERTY(QColor informationForeground READ informationForeground WRITE setInformationForeground)
    // background color for log text
    Q_PROPERTY(QColor debugBackground READ debugBackground WRITE setDebugBackground)
    Q_PROPERTY(QColor warningBackground READ warningBackground WRITE setWarningBackground)
    Q_PROPERTY(QColor criticalBackground READ criticalBackground WRITE setCriticalBackground)
    Q_PROPERTY(QColor fatalBackground READ fatalBackground WRITE setFatalBackground)
    Q_PROPERTY(QColor informationBackground READ informationBackground WRITE setInformationBackground)

public:
    explicit LogDisplayPage(QWidget* parent = nullptr);
    virtual ~LogDisplayPage() override;

    // ILogProcessor interface
    virtual void log(QtMsgType type, const QMessageLogContext& context, const QString& buf) override;

    // Read all buffered message, stop buffer and start page recording
    Q_SLOT void setBuffer(LogDisplayBuffer* buffer);

    QtMsgType filter() const;
    void setFilter(QtMsgType value);

    size_t maxCount() const;
    void setMaxCount(size_t value);

    QColor debugForeground() const;
    void setDebugForeground(QColor color);
    QColor warningForeground() const;
    void setWarningForeground(QColor color);
    QColor criticalForeground() const;
    void setCriticalForeground(QColor color);
    QColor fatalForeground() const;
    void setFatalForeground(QColor color);
    QColor informationForeground() const;
    void setInformationForeground(QColor color);

    QColor debugBackground() const;
    void setDebugBackground(QColor color);
    QColor warningBackground() const;
    void setWarningBackground(QColor color);
    QColor criticalBackground() const;
    void setCriticalBackground(QColor color);
    QColor fatalBackground() const;
    void setFatalBackground(QColor color);
    QColor informationBackground() const;
    void setInformationBackground(QColor color);
    // TODO changeEvent retranslateUi

private:
    friend class LogDisplayWidget;
    friend class LogDisplayWidgetPrivate;
    LogDisplayPagePrivate* d_ptr;
};

// widget for displaying log message
class LOG4QTSHARED_EXPORT LogDisplayWidget : public QWidget, public ILogProcessor
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LogDisplayWidget)

    /* All properties will be set to all pages, replacing their own value */
    // message with level under filter will not be processed, default is Information
    Q_PROPERTY(QtMsgType filter READ filter WRITE setFilter)
    // max count to be shown(before filter), default is 10000
    Q_PROPERTY(int maxCount READ maxCount WRITE setMaxCount)
    // foreground color for log text
    Q_PROPERTY(QColor debugForeground READ debugForeground WRITE setDebugForeground)
    Q_PROPERTY(QColor warningForeground READ warningForeground WRITE setWarningForeground)
    Q_PROPERTY(QColor criticalForeground READ criticalForeground WRITE setCriticalForeground)
    Q_PROPERTY(QColor fatalForeground READ fatalForeground WRITE setFatalForeground)
    Q_PROPERTY(QColor informationForeground READ informationForeground WRITE setInformationForeground)
    // background color for log text
    Q_PROPERTY(QColor debugBackground READ debugBackground WRITE setDebugBackground)
    Q_PROPERTY(QColor warningBackground READ warningBackground WRITE setWarningBackground)
    Q_PROPERTY(QColor criticalBackground READ criticalBackground WRITE setCriticalBackground)
    Q_PROPERTY(QColor fatalBackground READ fatalBackground WRITE setFatalBackground)
    Q_PROPERTY(QColor informationBackground READ informationBackground WRITE setInformationBackground)

public:
    explicit LogDisplayWidget(QWidget* parent = nullptr);
    virtual ~LogDisplayWidget() override;

    // ILogProcessor interface
    virtual void log(QtMsgType type, const QMessageLogContext& context, const QString& buf) override;

    // Read all buffered message, stop buffer and start widget recording
    Q_SLOT void setBuffer(LogDisplayBuffer* buffer);

    // get page of given category, has same properties as log4qtDisplayWidget
    LogDisplayPage* findPage(const QString& category) const;

    QtMsgType filter() const;
    void setFilter(QtMsgType value);

    int maxCount() const;
    void setMaxCount(int value);

    QColor debugForeground() const;
    void setDebugForeground(QColor color);
    QColor warningForeground() const;
    void setWarningForeground(QColor color);
    QColor criticalForeground() const;
    void setCriticalForeground(QColor color);
    QColor fatalForeground() const;
    void setFatalForeground(QColor color);
    QColor informationForeground() const;
    void setInformationForeground(QColor color);

    QColor debugBackground() const;
    void setDebugBackground(QColor color);
    QColor warningBackground() const;
    void setWarningBackground(QColor color);
    QColor criticalBackground() const;
    void setCriticalBackground(QColor color);
    QColor fatalBackground() const;
    void setFatalBackground(QColor color);
    QColor informationBackground() const;
    void setInformationBackground(QColor color);

protected:
    virtual void changeEvent(QEvent* event) override;

private:
    void retranslateUi();

    LogDisplayWidgetPrivate* d_ptr;
};
} // namespace log4qt
