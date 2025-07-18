#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QListView>
#include <QStandardItemModel>
#include <QAction>

#include "senderthread.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addCustomer();
    void addItem();
    void recordTransaction();
    void restoreItems();
    void broadcastData();

private:
    QTreeView *treeView;
    QListView *itemListView;

    QStandardItemModel *model;
    QStandardItemModel *itemModel;
    QStandardItemModel *itemBackup;

    QAction *addCustomerAction;
    QAction *addItemAction;
    QAction *recordTransactionAction;
    QAction *restoreItemsAction;
    QAction *broadcastAction;

    void setupMenus();
    void setupToolbar();
    void setupTree();
    void setupItemListView();
    void backupItems();
};

#endif // MAINWINDOW_H
