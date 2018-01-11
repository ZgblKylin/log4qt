#include <QtWidgets/QMenu>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QDialog>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QInputDialog>
#include "log4qtDisplayPage.h"
#include "log4qtDisplayModel.h"
#include "log4qtDisplayFilter.h"


log4qtDisplayPage::log4qtDisplayPage(QWidget* parent)
    : QTableView(parent)
{
    // initialize table
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::NoSelection);
    setWordWrap(false);
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->setResizeContentsPrecision(0);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setVisible(false);

    // initilize model
    model = new log4qtDisplayModel(this);
    filterModel = new log4qtDisplayFilter(model, this);
    setModel(filterModel);

    // initilize context menu
    menu = new QMenu(this);
    menu->addAction(tr("Dispaly level &filter"), this, SLOT(on_setDisplayFilter_triggered()));
    menu->addAction(tr("Select &column"), this, SLOT(on_selectColumn_triggered()));
    menu->addAction(tr("C&lear"), model, SLOT(clear()));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableView::customContextMenuRequested,
            [&](const QPoint &pos){ menu->popup(qobject_cast<QWidget*>(sender())->mapToGlobal(pos)); });
}

void log4qtDisplayPage::log(const QSharedPointer<log4qt::impl::LogMessage> message)
{
    // record current scrollbar position
    QScrollBar* scrollBar = verticalScrollBar();
    bool atBegin = scrollBar->value() == scrollBar->minimum();
    bool atEnd = scrollBar->value() == scrollBar->maximum();

    // add log message
    model->newLog(message);

    // restore scrollbar position
    if(atBegin)
        scrollToTop();
    if(atEnd)
        scrollToBottom();

    update();
}

void log4qtDisplayPage::on_setDisplayFilter_triggered()
{
    QInputDialog dialog(this);
    dialog.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    dialog.setWindowTitle(tr("Set display level filter"));
    dialog.setLabelText(tr("Set minimum display level(%1:%2-%3 %4:%5-%6 %7:%8-%9 %10:%11-%12 %13:%15)")
                        .arg(log4qt::impl::getLogTypeString(log4qt::impl::Debug)).arg(int(log4qt::Debug)).arg(int(log4qt::Infomation)-1)
                        .arg(log4qt::impl::getLogTypeString(log4qt::impl::Infomation)).arg(int(log4qt::Infomation)).arg(int(log4qt::Warning)-1)
                        .arg(log4qt::impl::getLogTypeString(log4qt::impl::Warning)).arg(int(log4qt::Warning)).arg(int(log4qt::Critical)-1)
                        .arg(log4qt::impl::getLogTypeString(log4qt::impl::Critical)).arg(int(log4qt::Critical)).arg(int(log4qt::Fatal)-1)
                        .arg(log4qt::impl::getLogTypeString(log4qt::impl::Fatal)).arg(int(log4qt::Fatal)));
    dialog.setInputMode(QInputDialog::IntInput);
    dialog.setIntRange(0, 100);
    dialog.setIntValue(filterModel->getFilter());
    if(dialog.exec() == QDialog::Accepted)
    filterModel->setFilter(dialog.intValue());
}

void log4qtDisplayPage::on_selectColumn_triggered()
{
    // initilize dialog
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Select column"));
    dialog.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    // layout for dialog
    QVBoxLayout* layout = new QVBoxLayout;
    dialog.setLayout(layout);

    // checkboxes for each column
    QGridLayout* checkLayout = new QGridLayout;
    layout->addItem(checkLayout);
    QList<QCheckBox*> checkBoxes;
    for(int col=0;col<model->columnCount();++col)
    {
        QCheckBox* checkBox = new QCheckBox(
                                  model->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString(),
                                  &dialog);
        checkBox->setChecked(filterModel->columnEnabled(col));
        checkBoxes << checkBox;
        checkLayout->addWidget(checkBox, col/3, col%3);
    }

    // buttonbox
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // execute dialog
    if(dialog.exec() == QDialog::Rejected)
        return;

    // update column states
    for(int i=0;i<checkBoxes.count();++i)
        filterModel->setColumnEnabled(i, checkBoxes.at(i)->isChecked());
}

int log4qtDisplayPage::filter() const
{
    return filterModel->getFilter();
}

void log4qtDisplayPage::setFilter(int value)
{
    filterModel->setFilter(value);
}

int log4qtDisplayPage::maxCount() const
{
    return model->getMaxCount();
}

void log4qtDisplayPage::setMaxCount(int value)
{
    model->setMaxCount(value);
}

QColor log4qtDisplayPage::debugForeground() const
{
    return model->property("debugForeground").value<QColor>();
}

void log4qtDisplayPage::setDebugForeground(QColor color)
{
    model->setProperty("debugForeground", color);
}

QColor log4qtDisplayPage::infomationForeground() const
{
    return model->property("infomationForeground").value<QColor>();
}

void log4qtDisplayPage::setInfomationForeground(QColor color)
{
    model->setProperty("infomationForeground", color);
}

QColor log4qtDisplayPage::warningForeground() const
{
    return model->property("warningForeground").value<QColor>();
}

void log4qtDisplayPage::setWarningForeground(QColor color)
{
    model->setProperty("warningForeground", color);
}

QColor log4qtDisplayPage::criticalForeground() const
{
    return model->property("criticalForeground").value<QColor>();
}

void log4qtDisplayPage::setCriticalForeground(QColor color)
{
    model->setProperty("criticalForeground", color);
}

QColor log4qtDisplayPage::fatalForeground() const
{
    return model->property("fatalForeground").value<QColor>();
}

void log4qtDisplayPage::setFatalForeground(QColor color)
{
    model->setProperty("fatalForeground", color);
}

QColor log4qtDisplayPage::debugBackground() const
{
    return model->property("debugBackground").value<QColor>();
}

void log4qtDisplayPage::setDebugBackground(QColor color)
{
    model->setProperty("debugBackground", color);
}

QColor log4qtDisplayPage::infomationBackground() const
{
    return model->property("infomationBackground").value<QColor>();
}

void log4qtDisplayPage::setInfomationBackground(QColor color)
{
    model->setProperty("infomationBackground", color);
}

QColor log4qtDisplayPage::warningBackground() const
{
    return model->property("warningBackground").value<QColor>();
}

void log4qtDisplayPage::setWarningBackground(QColor color)
{
    model->setProperty("warningBackground", color);
}

QColor log4qtDisplayPage::criticalBackground() const
{
    return model->property("criticalBackground").value<QColor>();
}

void log4qtDisplayPage::setCriticalBackground(QColor color)
{
    model->setProperty("criticalBackground", color);
}

QColor log4qtDisplayPage::fatalBackground() const
{
    return model->property("fatalBackground").value<QColor>();
}

void log4qtDisplayPage::setFatalBackground(QColor color)
{
    model->setProperty("fatalBackground", color);
}
