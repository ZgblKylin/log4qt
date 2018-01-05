#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QStackedLayout>
#include <processors/log4qtDisplayWidget.h>
#include "log4qtDisplayPage.h"


/* ================ LogDisplayBuffer ================ */
LogDisplayBuffer::LogDisplayBuffer(QObject* parent) :
    log4qt::impl::ILogProcessor(parent)
{
}

QList<QSharedPointer<log4qt::impl::LogMessage>> LogDisplayBuffer::takeAll()
{
    log4qt::impl::getLogEngine()->unRegisterProcessor(this);
    auto ret = messages;
    messages.clear();
    return ret;
}

void LogDisplayBuffer::start()
{
    log4qt::impl::getLogEngine()->registerProcessor(this, Qt::QueuedConnection);
}

void LogDisplayBuffer::log(const QSharedPointer<log4qt::impl::LogMessage> message)
{
    messages << message;
}


/* ================ log4qtDisplayWidget ================ */
log4qtDisplayPage* createPage(const QString& category, QWidget* parent)
{
    log4qtDisplayPage* page = new log4qtDisplayPage(parent);
    page->setProperty("category", category);
    page->setProperty("filter", parent->property("filter"));
    page->setProperty("maxCount", parent->property("maxCount"));
    page->setProperty("debugForeground", parent->property("debugForeground"));
    page->setProperty("infomationForeground", parent->property("infomationForeground"));
    page->setProperty("warningForeground", parent->property("warningForeground"));
    page->setProperty("criticalForeground", parent->property("criticalForeground"));
    page->setProperty("fatalForeground", parent->property("fatalForeground"));
    page->setProperty("debugBackground", parent->property("debugBackground"));
    page->setProperty("infomationBackground", parent->property("infomationBackground"));
    page->setProperty("warningBackground", parent->property("warningBackground"));
    page->setProperty("criticalBackground", parent->property("criticalBackground"));
    page->setProperty("fatalBackground", parent->property("fatalBackground"));
    return page;
}

void setAllPageProperty(const char* name, const QVariant& value, QStackedLayout* layout)
{
    for(int i = 0; i < layout->count(); ++i)
        layout->widget(i)->setProperty(name, value);
}

log4qtDisplayWidget::log4qtDisplayWidget(QWidget* parent)
    : log4qtDisplayWidget(0, parent)
{
}

log4qtDisplayWidget::log4qtDisplayWidget(LogDisplayBuffer* buffer, QWidget* parent)
    : log4qt::impl::ILogProcessorWidget<QGroupBox>(parent)
{
    // widget layout
    QVBoxLayout* layout = new QVBoxLayout(this);

    // title layout
    QHBoxLayout* titleLayout = new QHBoxLayout;
    layout->addLayout(titleLayout);

    // left spacer in title layout
    titleLayout->addItem(new QSpacerItem(467, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    // title
    QLabel* title = new QLabel(tr("log"), this);
    titleLayout->addWidget(title);

    // left button in title layout
    QToolButton* left = new QToolButton(this);
    left->setArrowType(Qt::LeftArrow);
    left->setAutoRaise(true);
    connect(left, &QToolButton::clicked,
            [=]{
        int index = stackedLayout->currentIndex() - 1;
        if(index < 0)
            index += stackedLayout->count();
        index = qBound(0, index, stackedLayout->count() - 1);
        stackedLayout->setCurrentIndex(index);
    });
    titleLayout->addWidget(left);

    // right button in title layout
    QToolButton* right = new QToolButton(this);
    right->setArrowType(Qt::RightArrow);
    right->setAutoRaise(true);
    connect(right, &QToolButton::clicked,
            [=]{
        int index = stackedLayout->currentIndex() + 1;
        index %= stackedLayout->count();
        index = qBound(0, index, stackedLayout->count() - 1);
        stackedLayout->setCurrentIndex(index);
    });
    titleLayout->addWidget(right);

    // right spacer in title layout
    titleLayout->addItem(new QSpacerItem(467, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    // stacked layout to contain category pages
    stackedLayout = new QStackedLayout(this);
    layout->addLayout(stackedLayout);
    connect(stackedLayout, &QStackedLayout::currentChanged,
            [=](int index){
        log4qtDisplayPage* page = qobject_cast<log4qtDisplayPage*>(stackedLayout->widget(index));
        QString text = tr("Log");
        if(!page->property("category").toString().isEmpty())
            text = tr("Log - %1").arg(page->property("category").toString());
        title->setText(text);
    });
    stackedLayout->addWidget(createPage(QString(), this));

    // change objectName for all children
    connect(this, &QObject::objectNameChanged,
            [=](const QString &objectName){
        layout->setObjectName(objectName + "_layout");
        titleLayout->setObjectName(objectName + "_titleLayout");
        stackedLayout->setObjectName(objectName + "_stackedLayout");
        title->setObjectName(objectName + "_title");
        left->setObjectName(objectName + "_leftButton");
        right->setObjectName(objectName + "_rightButton");
        for(int i = 0; i < stackedLayout->count(); ++i)
        {
            log4qtDisplayPage* page = qobject_cast<log4qtDisplayPage*>(stackedLayout->widget(i));
            page->setObjectName(objectName + "_" + page->property("category").toString());
        }
    });
    setObjectName("log4qtDisplayWidget");

    // display log messages in buffer
    if(buffer)
    {
        for(auto message : buffer->takeAll())
            log(message);
    }
}

QWidget* log4qtDisplayWidget::getPage(const QString& category) const
{
    for(int i = 0; i < stackedLayout->count(); ++i)
    {
        QWidget* page = stackedLayout->widget(i);
        if(page->property("category").toString() == category)
            return page;
    }
    return nullptr;
}

void log4qtDisplayWidget::start()
{
    log4qt::impl::getLogEngine()->registerProcessor(this, Qt::QueuedConnection);
}

void log4qtDisplayWidget::log(const QSharedPointer<log4qt::impl::LogMessage> message)
{
    log4qtDisplayPage* page = qobject_cast<log4qtDisplayPage*>(getPage(message->category));
    if(!page)
    {
        page = createPage(message->category, this);
        stackedLayout->addWidget(page);
    }
    page->log(message);
}

int log4qtDisplayWidget::getFilter() const
{
    return filter;
}

void log4qtDisplayWidget::setFilter(int value)
{
    filter = value;
    setAllPageProperty("filter", filter, stackedLayout);
}

int log4qtDisplayWidget::getMaxCount() const
{
    return maxCount;
}

void log4qtDisplayWidget::setMaxCount(int value)
{
    maxCount = value;
    setAllPageProperty("maxCount", maxCount, stackedLayout);
}

QColor log4qtDisplayWidget::getDebugForeground() const
{
    return debugForeground;
}

void log4qtDisplayWidget::setDebugForeground(QColor color)
{
    debugForeground = color;
    setAllPageProperty("debugForeground", debugForeground, stackedLayout);
}

QColor log4qtDisplayWidget::getInfomationForeground() const
{
    return infomationForeground;
}

void log4qtDisplayWidget::setInfomationForeground(QColor color)
{
    infomationForeground = color;
    setAllPageProperty("infomationForeground", infomationForeground, stackedLayout);
}

QColor log4qtDisplayWidget::getWarningForeground() const
{
    return warningForeground;
}

void log4qtDisplayWidget::setWarningForeground(QColor color)
{
    warningForeground = color;
    setAllPageProperty("warningForeground", warningForeground, stackedLayout);
}

QColor log4qtDisplayWidget::getCriticalForeground() const
{
    return criticalForeground;
}

void log4qtDisplayWidget::setCriticalForeground(QColor color)
{
    criticalForeground = color;
    setAllPageProperty("criticalForeground", criticalForeground, stackedLayout);
}

QColor log4qtDisplayWidget::getFatalForeground() const
{
    return fatalForeground;
}

void log4qtDisplayWidget::setFatalForeground(QColor color)
{
    fatalForeground = color;
    setAllPageProperty("fatalForeground", fatalForeground, stackedLayout);
}

QColor log4qtDisplayWidget::getDebugBackground() const
{
    return debugBackground;
}

void log4qtDisplayWidget::setDebugBackground(QColor color)
{
    debugBackground = color;
    setAllPageProperty("debugBackground", debugBackground, stackedLayout);
}

QColor log4qtDisplayWidget::getInfomationBackground() const
{
    return infomationBackground;
}

void log4qtDisplayWidget::setInfomationBackground(QColor color)
{
    infomationBackground = color;
    setAllPageProperty("infomationBackground", infomationBackground, stackedLayout);
}

QColor log4qtDisplayWidget::getWarningBackground() const
{
    return warningBackground;
}

void log4qtDisplayWidget::setWarningBackground(QColor color)
{
    warningBackground = color;
    setAllPageProperty("warningBackground", warningBackground, stackedLayout);
}

QColor log4qtDisplayWidget::getCriticalBackground() const
{
    return criticalBackground;
}

void log4qtDisplayWidget::setCriticalBackground(QColor color)
{
    criticalBackground = color;
    setAllPageProperty("criticalBackground", criticalBackground, stackedLayout);
}

QColor log4qtDisplayWidget::getFatalBackground() const
{
    return fatalBackground;
}

void log4qtDisplayWidget::setFatalBackground(QColor color)
{
    fatalBackground = color;
    setAllPageProperty("fatalBackground", fatalBackground, stackedLayout);
}
