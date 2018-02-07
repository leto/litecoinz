// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "unspentdialog.h"
#include "ui_unspentdialog.h"

#include "unspenttablemodel.h"
#include "bitcoingui.h"
#include "guiutil.h"
#include "platformstyle.h"

#include <QSortFilterProxyModel>

UnspentDialog::UnspentDialog(const PlatformStyle *platformStyle, Mode mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UnspentDialog),
    model(0),
    mode(mode)
{
    ui->setupUi(this);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    switch(mode)
    {
    case ForSelection:
        ui->closeButton->setText(tr("C&hoose"));
        break;
    case ForEditing:
        ui->closeButton->setText(tr("C&lose"));
        break;
    }

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()));
}

UnspentDialog::~UnspentDialog()
{
    delete ui;
}

void UnspentDialog::setModel(UnspentTableModel *model)
{
    this->model = model;
    if(!model)
        return;

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->tableView->setModel(proxyModel);
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);

    // Set column widths
#if QT_VERSION < 0x050000
    ui->tableView->horizontalHeader()->setResizeMode(UnspentTableModel::Address, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setResizeMode(UnspentTableModel::Balance, QHeaderView::ResizeToContents);
#else
    ui->tableView->horizontalHeader()->setSectionResizeMode(UnspentTableModel::Address, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(UnspentTableModel::Balance, QHeaderView::ResizeToContents);
#endif

    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(selectionChanged()));

    selectionChanged();
}

void UnspentDialog::selectionChanged()
{
    // Set button states based on selected tab and selection
    QTableView *table = ui->tableView;

    if(!table->selectionModel())
        return;
}

void UnspentDialog::done(int retval)
{
    QTableView *table;
    QModelIndexList indexes;

    table = ui->tableView;
    if(!table->selectionModel() || !table->model()) {
        return;
    }
    indexes = table->selectionModel()->selectedRows(UnspentTableModel::Address);

    // Figure out which address was selected, and return it
    Q_FOREACH (const QModelIndex& index, indexes) {
        QVariant address = table->model()->data(index);
        returnValue = address.toString();
    }

    if(returnValue.isEmpty())
    {
        // If no address entry selected, return rejected
        retval = Rejected;
    }

    QDialog::done(retval);
}
