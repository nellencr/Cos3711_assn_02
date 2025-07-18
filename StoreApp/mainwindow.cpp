#include "mainwindow.h"
#include "senderthread.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QInputDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QDockWidget>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    model = new QStandardItemModel(this);
    itemModel = new QStandardItemModel(this);
    itemBackup = new QStandardItemModel(this);

    setupMenus();
    setupToolbar();
    setupTree();
    setupItemListView();

    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow() {}

void MainWindow::setupMenus() {
    QMenu *fileMenu = menuBar()->addMenu("File");
    restoreItemsAction = fileMenu->addAction("Restore Items");
    connect(restoreItemsAction, &QAction::triggered, this, &MainWindow::restoreItems);

    QMenu *customerMenu = menuBar()->addMenu("Customer");
    addCustomerAction = customerMenu->addAction("Add Customer");
    connect(addCustomerAction, &QAction::triggered, this, &MainWindow::addCustomer);

    QMenu *itemMenu = menuBar()->addMenu("Items");
    addItemAction = itemMenu->addAction("Add Item");
    connect(addItemAction, &QAction::triggered, this, &MainWindow::addItem);

    QMenu *transMenu = menuBar()->addMenu("Transactions");
    recordTransactionAction = transMenu->addAction("Record Transaction");
    connect(recordTransactionAction, &QAction::triggered, this, &MainWindow::recordTransaction);

    QMenu *networkMenu = menuBar()->addMenu("Network");
    broadcastAction = networkMenu->addAction("Broadcast Data");
    connect(broadcastAction, &QAction::triggered, this, &MainWindow::broadcastData);
}

void MainWindow::setupToolbar() {
    QToolBar *toolbar = addToolBar("Main Toolbar");
    toolbar->addAction(addCustomerAction);
    toolbar->addAction(addItemAction);
    toolbar->addAction(recordTransactionAction);
    toolbar->addAction(restoreItemsAction);
    toolbar->addAction(broadcastAction);
}

void MainWindow::setupTree() {
    treeView = new QTreeView(this);
    treeView->setModel(model);
    setCentralWidget(treeView);
    model->setHorizontalHeaderLabels(QStringList() << "Customer" << "Transaction");
}

void MainWindow::setupItemListView() {
    itemListView = new QListView(this);
    itemListView->setModel(itemModel);

    QDockWidget *dock = new QDockWidget("Items", this);
    dock->setWidget(itemListView);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::addCustomer() {
    bool ok;
    QString name = QInputDialog::getText(this, "Add Customer", "Customer Name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        QList<QStandardItem *> row;
        row << new QStandardItem(name) << new QStandardItem("");
        model->appendRow(row);
    }
}

void MainWindow::addItem() {
    bool ok;
    QStringList types = { "Book", "Magazine" };
    QString type = QInputDialog::getItem(this, "Add Item", "Type:", types, 0, false, &ok);
    if (!ok || type.isEmpty()) return;

    QString name = QInputDialog::getText(this, "Add Item", "Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QString fullItem = QString("%1: %2").arg(type, name);
    itemModel->appendRow(new QStandardItem(fullItem));
    backupItems();
}

void MainWindow::recordTransaction() {
    QStringList customerNames;
    for (int i = 0; i < model->rowCount(); ++i) {
        customerNames << model->item(i)->text();
    }

    if (customerNames.isEmpty()) {
        QMessageBox::warning(this, "No Customers", "Please add a customer first.");
        return;
    }

    bool okCustomer;
    QString customerName = QInputDialog::getItem(this, "Select Customer",
                                                 "Customer:", customerNames, 0, false, &okCustomer);
    if (!okCustomer || customerName.isEmpty()) return;

    QStringList itemNames;
    for (int i = 0; i < itemModel->rowCount(); ++i) {
        itemNames << itemModel->item(i)->text();
    }

    if (itemNames.isEmpty()) {
        QMessageBox::warning(this, "No Items", "Please add an item first.");
        return;
    }

    bool okItem;
    QString itemName = QInputDialog::getItem(this, "Select Item",
                                             "Item:", itemNames, 0, false, &okItem);
    if (!okItem || itemName.isEmpty()) return;

    bool okQty;
    int quantity = QInputDialog::getInt(this, "Quantity", "Enter quantity:", 1, 1, 1000, 1, &okQty);
    if (!okQty) return;

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
    QString transText = QString("%1 x%2 - [%3]").arg(itemName).arg(quantity).arg(timestamp);

    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *custItem = model->item(i);
        if (custItem->text() == customerName) {
            QStandardItem *transItem = new QStandardItem(transText);
            custItem->appendRow(QList<QStandardItem*>() << new QStandardItem("") << transItem);
            treeView->expandAll();
            statusBar()->showMessage("Transaction recorded for " + customerName);
            return;
        }
    }

    QMessageBox::warning(this, "Error", "Could not find customer.");
}

void MainWindow::backupItems() {
    itemBackup->clear();
    for (int i = 0; i < itemModel->rowCount(); ++i) {
        QStandardItem *item = itemModel->item(i)->clone();
        itemBackup->appendRow(item);
    }
}

void MainWindow::restoreItems() {
    itemModel->clear();
    for (int i = 0; i < itemBackup->rowCount(); ++i) {
        QStandardItem *item = itemBackup->item(i)->clone();
        itemModel->appendRow(item);
    }
}

void MainWindow::broadcastData() {
    SenderThread *thread = new SenderThread(model);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();

    QMessageBox::information(this, "Broadcast", "Transaction data broadcasted via UDP.");
}

