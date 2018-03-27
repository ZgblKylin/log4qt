#ifndef LOG4QTDISPLAYWIDGET_H
#define LOG4QTDISPLAYWIDGET_H

#include <QtCore/QVector>
#include <QtGui/QColor>
#include <QtWidgets/QGroupBox>
#include <log4qt.h>

class QStackedLayout;

// buffer for caching log messages before log4qtDisplayWidget constructed
class LOG4QTSHARED_EXPORT LogDisplayBuffer : public log4qt::impl::ILogProcessor
{
    Q_OBJECT
public:
    explicit LogDisplayBuffer(QObject* parent = 0);
    virtual ~LogDisplayBuffer() = default;

    // take all stored log messages, stop caching log
    QVector<QSharedPointer<log4qt::impl::LogMessage>> takeAll();

    // ILogProcessor interface
    virtual void start() override;
    Q_SLOT virtual void log(const QSharedPointer<log4qt::impl::LogMessage> message) override;

private:
    QVector<QSharedPointer<log4qt::impl::LogMessage>> messages;
};


// widget for displaying log message
class LOG4QTSHARED_EXPORT log4qtDisplayWidget : public log4qt::impl::ILogProcessorWidget<QGroupBox>
{
    Q_OBJECT

    /* ======== All properties will be set to all pages, replacing their own value ======== */
    // message with level under filter will not be processed, default is Infomation
    Q_PROPERTY(int getFilter READ getFilter WRITE setFilter)
    // max count to be shown(before filter), default is 10000
    Q_PROPERTY(int getMaxCount READ getMaxCount WRITE setMaxCount)
    // foreground color for log text
    Q_PROPERTY(QColor debugForeground READ getDebugForeground WRITE setDebugForeground)
    Q_PROPERTY(QColor infomationForeground READ getInfomationForeground WRITE setInfomationForeground)
    Q_PROPERTY(QColor warningForeground READ getWarningForeground WRITE setWarningForeground)
    Q_PROPERTY(QColor criticalForeground READ getCriticalForeground WRITE setCriticalForeground)
    Q_PROPERTY(QColor fatalForeground READ getFatalForeground WRITE setFatalForeground)
    // background color for log text
    Q_PROPERTY(QColor debugBackground READ getDebugBackground WRITE setDebugBackground)
    Q_PROPERTY(QColor infomationBackground READ getInfomationBackground WRITE setInfomationBackground)
    Q_PROPERTY(QColor warningBackground READ getWarningBackground WRITE setWarningBackground)
    Q_PROPERTY(QColor criticalBackground READ getCriticalBackground WRITE setCriticalBackground)
    Q_PROPERTY(QColor fatalBackground READ getFatalBackground WRITE setFatalBackground)

public:
    explicit log4qtDisplayWidget(QWidget* parent = 0);
    log4qtDisplayWidget(LogDisplayBuffer* buffer, QWidget* parent = 0);
    virtual ~log4qtDisplayWidget() = default;

    // get page of given category, has same properties as log4qtDisplayWidget
    QWidget* getPage(const QString& category) const;

    // ILogProcessor interface
    virtual void start() override;
    Q_SLOT virtual void log(const QSharedPointer<log4qt::impl::LogMessage> message) override;

protected:
    virtual void changeEvent(QEvent* event) override;

private:
    void retranslateUi();

    int getFilter() const;
    void setFilter(int value);

    int getMaxCount() const;
    void setMaxCount(int value);

    QColor getDebugForeground() const;
    void setDebugForeground(QColor color);
    QColor getInfomationForeground() const;
    void setInfomationForeground(QColor color);
    QColor getWarningForeground() const;
    void setWarningForeground(QColor color);
    QColor getCriticalForeground() const;
    void setCriticalForeground(QColor color);
    QColor getFatalForeground() const;
    void setFatalForeground(QColor color);

    QColor getDebugBackground() const;
    void setDebugBackground(QColor color);
    QColor getInfomationBackground() const;
    void setInfomationBackground(QColor color);
    QColor getWarningBackground() const;
    void setWarningBackground(QColor color);
    QColor getCriticalBackground() const;
    void setCriticalBackground(QColor color);
    QColor getFatalBackground() const;
    void setFatalBackground(QColor color);

    // properties
    int filter = log4qt::Infomation;
    int maxCount = 10000;
    QColor debugForeground = Qt::white;
    QColor infomationForeground = Qt::white;
    QColor warningForeground = Qt::white;
    QColor criticalForeground = Qt::white;
    QColor fatalForeground = Qt::white;
    QColor debugBackground = QColor::fromRgb(0x6C7EC2);
    QColor infomationBackground = QColor::fromRgb(0x50BC6C);
    QColor warningBackground = QColor::fromRgb(0xDE9E24);
    QColor criticalBackground = QColor::fromRgb(0xD73333);
    QColor fatalBackground = QColor::fromRgb(0x591111);

    QStackedLayout* stackedLayout;
};

#endif // LOG4QTDISPLAYWIDGET_H
