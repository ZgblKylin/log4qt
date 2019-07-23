#include "LogDisplayWidgetPrivate.h"

namespace log4qt {
/* ================ LogDisplayBuffer ================ */
LogDisplayBuffer::LogDisplayBuffer(QObject* parent)
    : QObject(parent),
      d_ptr(new LogDisplayBufferPrivate(this))
{
}

LogDisplayBuffer::~LogDisplayBuffer()
{
}

void LogDisplayBuffer::log(QtMsgType type, const QMessageLogContext& context, const QString& buf)
{
    Q_D(LogDisplayBuffer);
    while (!d->mutex.tryLock()) {}
    defer [d]{ d->mutex.unlock(); };
    d->messages.emplace_back(type, context, buf);
}

std::vector<LogMessage> LogDisplayBuffer::takeAll()
{
    Q_D(LogDisplayBuffer);
    while (!d->mutex.tryLock()) {}
    defer [d]{ d->mutex.unlock(); };
    LogEngine::unRegisterProcessor(this);
    return std::move(d->messages);
}
/* ================ LogDisplayBuffer ================ */

/* ================ LogDisplayPage ================ */
void LogDisplayPagePrivate::log(LogMessage message)
{
    Q_Q(LogDisplayPage);

    // record current scrollbar position
    QScrollBar* scrollBar = table->verticalScrollBar();
    bool atBegin = scrollBar->value() == scrollBar->minimum();
    bool atEnd = scrollBar->value() == scrollBar->maximum();

    // add log message
    dataModel->log(std::move(message));

    // restore scrollbar position
    if(atBegin) table->scrollToTop();
    if(atEnd) table->scrollToBottom();

    q->update();
}

void LogDisplayPagePrivate::onSetDisplayFilter()
{
    Q_Q(LogDisplayPage);
    static const QStringList items = {
        typeToString(QtDebugMsg),
        typeToString(QtWarningMsg),
        typeToString(QtCriticalMsg),
        typeToString(QtFatalMsg),
        typeToString(QtInfoMsg)
    };
    bool ok = false;
    QString item = QInputDialog::getItem(
                       q,
                       LogDisplayPage::tr("Set display level filter"),
                       LogDisplayPage::tr("Set minimum display level"),
                       items,
                       int(filterModel->filter()),
                       false,
                       &ok);
    if (!ok) return;
    int type = items.indexOf(item);
    filterModel->setFilter(QtMsgType(type));
}

void LogDisplayPagePrivate::onSelectColumn()
{
    Q_Q(LogDisplayPage);

    // initilize dialog
    QDialog dialog(q);
    dialog.setWindowTitle(QStringLiteral("Select column"));
    dialog.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    // layout for dialog
    QVBoxLayout* layout = new QVBoxLayout;
    dialog.setLayout(layout);

    // checkboxes for each column
    QGridLayout* checkLayout = new QGridLayout;
    layout->addItem(checkLayout);
    QList<QCheckBox*> checkBoxes;
    for(int col = 0; col < dataModel->columnCount(); ++col)
    {
        QCheckBox* checkBox = new QCheckBox(
                                  dataModel->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString(),
                                  &dialog);
        checkBox->setChecked(filterModel->columnEnabled(col));
        checkBoxes << checkBox;
        checkLayout->addWidget(checkBox, col/3, col%3);
    }

    // buttonbox
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttonBox);
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // execute dialog
    if(dialog.exec() == QDialog::Rejected) return;

    // update column states
    for(int i=0;i<checkBoxes.count();++i) {
        filterModel->setColumnEnabled(i, checkBoxes.at(i)->isChecked());
    }
}

LogDisplayPage::LogDisplayPage(QWidget* parent)
    : QWidget(parent),
      d_ptr(new LogDisplayPagePrivate(this))
{
    Q_D(LogDisplayPage);

    // initialize table
    d->widgetLayout = new QHBoxLayout(this);
    d->table = new QTableView(this);
    d->widgetLayout->addWidget(d->table);
    d->table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d->table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->table->setSelectionMode(QAbstractItemView::NoSelection);
    d->table->setWordWrap(false);
    d->table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    d->table->horizontalHeader()->setResizeContentsPrecision(0);
    d->table->horizontalHeader()->setStretchLastSection(true);
    d->table->verticalHeader()->setVisible(false);

    // initilize model
    d->dataModel = new LogDisplayModel(this);
    d->filterModel = new LogDisplayFilter(d->dataModel, this);
    d->table->setModel(d->filterModel);

    // initilize context menu
    d->menu = new QMenu(this);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableView::customContextMenuRequested,
            [this, d](const QPoint &pos){
        d->menu->popup(qobject_cast<QWidget*>(sender())->mapToGlobal(pos));
    });

    d->actionSetDisplayFilter = new QAction(tr("Dispaly level &filter"));
    connect(d->actionSetDisplayFilter, &QAction::triggered,
            this, [=]{ d->onSetDisplayFilter(); });
    d->menu->addAction(d->actionSetDisplayFilter);

    d->actionSelectColumn = new QAction(tr("Select &section"));
    connect(d->actionSelectColumn, &QAction::triggered,
            this, [=]{ d->onSelectColumn(); });
    d->menu->addAction(d->actionSelectColumn);

    d->actionClear = d->menu->addAction(tr("&Clear"),
                                        d->dataModel,
                                        SLOT(clear()));

    connect(&(d->sender), &LogDisplaySignalSender::log,
            this, [=](LogMessage message){
        d->log(std::move(message));
    }, Qt::QueuedConnection);
}

LogDisplayPage::~LogDisplayPage()
{
}

void LogDisplayPage::log(QtMsgType type, const QMessageLogContext& context, const QString& buf)
{
    Q_D(LogDisplayPage);
    LogMessage message(type, context, buf);
    if (message.category.isEmpty()) message.category = QLoggingCategory::defaultCategory()->categoryName();
    emit d->sender.log(std::move(message));
}

void LogDisplayPage::setBuffer(LogDisplayBuffer* buffer)
{
    Q_D(LogDisplayPage);
    for (LogMessage& message : buffer->takeAll())
    {
        if (message.category.isEmpty()) message.category = QLoggingCategory::defaultCategory()->categoryName();
        emit d->sender.log(std::move(message));
    }
    start();
}

QtMsgType LogDisplayPage::filter() const
{
    Q_D(const LogDisplayPage);
    return d->filterModel->filter();
}

void LogDisplayPage::setFilter(QtMsgType value)
{
    Q_D(LogDisplayPage);
    d->filterModel->setFilter(value);
}

size_t LogDisplayPage::maxCount() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->maxCount();
}

void LogDisplayPage::setMaxCount(size_t value)
{
    Q_D(LogDisplayPage);
    d->dataModel->setMaxCount(value);
}

QColor LogDisplayPage::debugForeground() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->property("debugForeground").value<QColor>();
}

void LogDisplayPage::setDebugForeground(QColor color)
{
    Q_D(LogDisplayPage);
    d->dataModel->setProperty("debugForeground", color);
}

QColor LogDisplayPage::warningForeground() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->property("warningForeground").value<QColor>();
}

void LogDisplayPage::setWarningForeground(QColor color)
{
    Q_D(LogDisplayPage);
    d->dataModel->setProperty("warningForeground", color);
}

QColor LogDisplayPage::criticalForeground() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->property("criticalForeground").value<QColor>();
}

void LogDisplayPage::setCriticalForeground(QColor color)
{
    Q_D(LogDisplayPage);
    d->dataModel->setProperty("criticalForeground", color);
}

QColor LogDisplayPage::fatalForeground() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->property("fatalForeground").value<QColor>();
}

void LogDisplayPage::setFatalForeground(QColor color)
{
    Q_D(LogDisplayPage);
    d->dataModel->setProperty("fatalForeground", color);
}

QColor LogDisplayPage::informationForeground() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->property("informationForeground").value<QColor>();
}

void LogDisplayPage::setInformationForeground(QColor color)
{
    Q_D(LogDisplayPage);
    d->dataModel->setProperty("informationForeground", color);
}

QColor LogDisplayPage::debugBackground() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->property("debugBackground").value<QColor>();
}

void LogDisplayPage::setDebugBackground(QColor color)
{
    Q_D(LogDisplayPage);
    d->dataModel->setProperty("debugBackground", color);
}

QColor LogDisplayPage::warningBackground() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->property("warningBackground").value<QColor>();
}

void LogDisplayPage::setWarningBackground(QColor color)
{
    Q_D(LogDisplayPage);
    d->dataModel->setProperty("warningBackground", color);
}

QColor LogDisplayPage::criticalBackground() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->property("criticalBackground").value<QColor>();
}

void LogDisplayPage::setCriticalBackground(QColor color)
{
    Q_D(LogDisplayPage);
    d->dataModel->setProperty("criticalBackground", color);
}

QColor LogDisplayPage::fatalBackground() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->property("fatalBackground").value<QColor>();
}

void LogDisplayPage::setFatalBackground(QColor color)
{
    Q_D(LogDisplayPage);
    d->dataModel->setProperty("fatalBackground", color);
}

QColor LogDisplayPage::informationBackground() const
{
    Q_D(const LogDisplayPage);
    return d->dataModel->property("informationBackground").value<QColor>();
}

void LogDisplayPage::setInformationBackground(QColor color)
{
    Q_D(LogDisplayPage);
    d->dataModel->setProperty("informationBackground", color);
}
/* ================ LogDisplayPage ================ */

/* ================ LogDisplayWidget ================ */
LogDisplayPage* LogDisplayWidgetPrivate::createPage(const QString& category)
{
    Q_Q(LogDisplayWidget);
    QString temp = category;
    if (temp.isEmpty()) {
        temp = QLoggingCategory::defaultCategory()->categoryName();
    }
    LogDisplayPage* page = new LogDisplayPage(q);
    page->setProperty("category", QVariant::fromValue(temp));
    page->setProperty("filter", filter);
    page->setProperty("maxCount", maxCount);
    page->setProperty("debugForeground", debugForeground);
    page->setProperty("warningForeground", warningForeground);
    page->setProperty("criticalForeground", criticalForeground);
    page->setProperty("fatalForeground", fatalForeground);
    page->setProperty("informationForeground", informationForeground);
    page->setProperty("debugBackground", debugBackground);
    page->setProperty("warningBackground", warningBackground);
    page->setProperty("criticalBackground", criticalBackground);
    page->setProperty("fatalBackground", fatalBackground);
    page->setProperty("informationBackground", informationBackground);
    return page;

}

void LogDisplayWidgetPrivate::setAllTaskProperty(const char* name, const QVariant& value)
{
    for(int i = 0; i < stackedLayout->count(); ++i) {
        stackedLayout->widget(i)->setProperty(name, value);
    }
}

void LogDisplayWidgetPrivate::log(LogMessage message)
{
    Q_Q(LogDisplayWidget);
    if (message.category.isEmpty()) message.category = QLoggingCategory::defaultCategory()->categoryName();
    LogDisplayPage* page = q->findPage(message.category);
    if(!page)
    {
        page = createPage(message.category);
        stackedLayout->addWidget(page);
        if (stackedLayout->count() > 0)
        {
            leftButton->show();
            rightButton->show();
        }
        objectNameChanged(q->objectName());
    }
    LogDisplayPagePrivate* p = page->d_func();
    p->log(std::move(message));
}

void LogDisplayWidgetPrivate::objectNameChanged(const QString& objectName)
{
    widgetLayout->setObjectName(objectName + "_layout");
    titleLayout->setObjectName(objectName + "_titleLayout");
    stackedLayout->setObjectName(objectName + "_stackedLayout");
    leftButton->setObjectName(objectName + "_leftButton");
    title->setObjectName(objectName + "_title");
    rightButton->setObjectName(objectName + "_rightButton");
    for(int i = 0; i < stackedLayout->count(); ++i)
    {
        QWidget* page = stackedLayout->widget(i);
        page->setObjectName(objectName + "_" +
                            page->property("category").toString());
    }
}

LogDisplayWidget::LogDisplayWidget(QWidget* parent)
    : QWidget(parent),
      d_ptr(new LogDisplayWidgetPrivate(this))
{
    Q_D(LogDisplayWidget);
    // widget layout
    d->widgetLayout = new QVBoxLayout(this);

    // title layout
    d->titleLayout = new QHBoxLayout;
    d->widgetLayout->addLayout(d->titleLayout);

    // left spacer in title layout
    d->titleLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    // left button in title layout
    d->leftButton = new QToolButton(this);
    d->leftButton->setArrowType(Qt::LeftArrow);
    d->leftButton->setAutoRaise(true);
    connect(d->leftButton, &QToolButton::clicked,
            [=]{
        int index = d->stackedLayout->currentIndex() - 1;
        if(index < 0) index += d->stackedLayout->count();
        d->stackedLayout->setCurrentIndex(index);
    });
    d->titleLayout->addWidget(d->leftButton);
    d->leftButton->hide();

    // title
    d->title = new QLabel(this);
    d->titleLayout->addWidget(d->title);

    // right button in title layout
    d->rightButton = new QToolButton(this);
    d->rightButton->setArrowType(Qt::RightArrow);
    d->rightButton->setAutoRaise(true);
    connect(d->rightButton, &QToolButton::clicked,
            [=]{
        int index = d->stackedLayout->currentIndex() + 1;
        index %= d->stackedLayout->count();
        d->stackedLayout->setCurrentIndex(index);
    });
    d->titleLayout->addWidget(d->rightButton);
    d->rightButton->hide();

    // right spacer in title layout
    d->titleLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

    // stacked layout to contain category pages
    d->stackedLayout = new QStackedLayout;
    d->widgetLayout->addLayout(d->stackedLayout);
    d->stackedLayout->addWidget(d->createPage(QLoggingCategory::defaultCategory()->categoryName()));

    // change objectName for all children
    connect(this, &QObject::objectNameChanged,
            [=](const QString& objectName){ d->objectNameChanged(objectName); });
    setObjectName("LogDisplayWidget");

    connect(d->stackedLayout, &QStackedLayout::currentChanged,
            this, &LogDisplayWidget::retranslateUi);

    retranslateUi();

    connect(&(d->sender), &LogDisplaySignalSender::log,
            this, [=](LogMessage message){
        d->log(std::move(message));
    }, Qt::QueuedConnection);
}

LogDisplayWidget::~LogDisplayWidget()
{
}

void LogDisplayWidget::log(QtMsgType type, const QMessageLogContext& context, const QString& buf)
{
    Q_D(LogDisplayWidget);
    emit d->sender.log(LogMessage(type, context, buf));
}

void LogDisplayWidget::setBuffer(LogDisplayBuffer* buffer)
{
    Q_D(LogDisplayWidget);
    for (LogMessage& message : buffer->takeAll())
    {
        emit d->sender.log(std::move(message));
    }
    start();
}

LogDisplayPage* LogDisplayWidget::findPage(const QString& category) const
{
    Q_D(const LogDisplayWidget);
    QString temp = category;
    if (temp.isEmpty()) temp = QLoggingCategory::defaultCategory()->categoryName();
    for(int i = 0; i < d->stackedLayout->count(); ++i)
    {
        auto page = qobject_cast<LogDisplayPage*>(d->stackedLayout->widget(i));
        if(page->property("category") == temp) {
            return qobject_cast<LogDisplayPage*>(page);
        }
    }
    return nullptr;
}

QtMsgType LogDisplayWidget::filter() const
{
    Q_D(const LogDisplayWidget);
    return d->filter;
}

void LogDisplayWidget::setFilter(QtMsgType value)
{
    Q_D(LogDisplayWidget);
    d->filter = value;
    d->setAllTaskProperty("filter", d->filter);
}

int LogDisplayWidget::maxCount() const
{
    Q_D(const LogDisplayWidget);
    return d->maxCount;
}

void LogDisplayWidget::setMaxCount(int value)
{
    Q_D(LogDisplayWidget);
    d->maxCount = value;
    d->setAllTaskProperty("maxCount", d->maxCount);
}

QColor LogDisplayWidget::debugForeground() const
{
    Q_D(const LogDisplayWidget);
    return d->debugForeground;
}

void LogDisplayWidget::setDebugForeground(QColor color)
{
    Q_D(LogDisplayWidget);
    d->debugForeground = color;
    d->setAllTaskProperty("debugForeground", d->debugForeground);
}

QColor LogDisplayWidget::warningForeground() const
{
    Q_D(const LogDisplayWidget);
    return d->warningForeground;
}

void LogDisplayWidget::setWarningForeground(QColor color)
{
    Q_D(LogDisplayWidget);
    d->warningForeground = color;
    d->setAllTaskProperty("warningForeground", d->warningForeground);
}

QColor LogDisplayWidget::criticalForeground() const
{
    Q_D(const LogDisplayWidget);
    return d->criticalForeground;
}

void LogDisplayWidget::setCriticalForeground(QColor color)
{
    Q_D(LogDisplayWidget);
    d->criticalForeground = color;
    d->setAllTaskProperty("criticalForeground", d->criticalForeground);
}

QColor LogDisplayWidget::fatalForeground() const
{
    Q_D(const LogDisplayWidget);
    return d->fatalForeground;
}

void LogDisplayWidget::setFatalForeground(QColor color)
{
    Q_D(LogDisplayWidget);
    d->fatalForeground = color;
    d->setAllTaskProperty("fatalForeground", d->fatalForeground);
}

QColor LogDisplayWidget::informationForeground() const
{
    Q_D(const LogDisplayWidget);
    return d->informationForeground;
}

void LogDisplayWidget::setInformationForeground(QColor color)
{
    Q_D(LogDisplayWidget);
    d->informationForeground = color;
    d->setAllTaskProperty("informationForeground", d->informationForeground);
}

QColor LogDisplayWidget::debugBackground() const
{
    Q_D(const LogDisplayWidget);
    return d->debugBackground;
}

void LogDisplayWidget::setDebugBackground(QColor color)
{
    Q_D(LogDisplayWidget);
    d->debugBackground = color;
    d->setAllTaskProperty("debugBackground", d->debugBackground);
}

QColor LogDisplayWidget::warningBackground() const
{
    Q_D(const LogDisplayWidget);
    return d->warningBackground;
}

void LogDisplayWidget::setWarningBackground(QColor color)
{
    Q_D(LogDisplayWidget);
    d->warningBackground = color;
    d->setAllTaskProperty("warningBackground", d->warningBackground);
}

QColor LogDisplayWidget::criticalBackground() const
{
    Q_D(const LogDisplayWidget);
    return d->criticalBackground;
}

void LogDisplayWidget::setCriticalBackground(QColor color)
{
    Q_D(LogDisplayWidget);
    d->criticalBackground = color;
    d->setAllTaskProperty("criticalBackground", d->criticalBackground);
}

QColor LogDisplayWidget::fatalBackground() const
{
    Q_D(const LogDisplayWidget);
    return d->fatalBackground;
}

void LogDisplayWidget::setFatalBackground(QColor color)
{
    Q_D(LogDisplayWidget);
    d->fatalBackground = color;
    d->setAllTaskProperty("fatalBackground", d->fatalBackground);
}

QColor LogDisplayWidget::informationBackground() const
{
    Q_D(const LogDisplayWidget);
    return d->informationBackground;
}

void LogDisplayWidget::setInformationBackground(QColor color)
{
    Q_D(LogDisplayWidget);
    d->informationBackground = color;
    d->setAllTaskProperty("informationBackground", d->informationBackground);
}

void LogDisplayWidget::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if(event->type() == QEvent::LanguageChange)
        retranslateUi();
}

void LogDisplayWidget::retranslateUi()
{
    Q_D(LogDisplayWidget);
    auto page = qobject_cast<LogDisplayPage*>(d->stackedLayout->currentWidget());
    QString text = tr("Log");
    if(page->property("category") != QLoggingCategory::defaultCategory()->categoryName())
    {
        text = tr("Log - %1").arg(page->property("category").toString());
    }
    findChild<QLabel*>(objectName() + "_title")->setText(text);
}
/* ================ LogDisplayWidget ================ */
} // namespace log4qt
