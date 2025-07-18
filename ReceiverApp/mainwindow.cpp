#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDomDocument>
#include <QHostAddress>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList() << "Customer" << "Transaction");
    ui->treeView->setModel(model);

    // Setup UDP socket
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::Any, 45454, QUdpSocket::ShareAddress);
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::processPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(int(udpSocket->pendingDatagramSize()));
        udpSocket->readDatagram(datagram.data(), datagram.size());

        QDomDocument doc;
        if (!doc.setContent(datagram)) {
            QMessageBox::warning(this, "Error", "Invalid XML data received.");
            continue;
        }

        model->clear();
        model->setHorizontalHeaderLabels(QStringList() << "Customer" << "Transaction");

        QDomElement root = doc.documentElement();
        QDomNodeList customers = root.elementsByTagName("customer");
        for (int i = 0; i < customers.count(); ++i) {
            QDomElement custElem = customers.at(i).toElement();
            QString custName = custElem.attribute("name");

            QList<QStandardItem *> custRow;
            QStandardItem *custItem = new QStandardItem(custName);
            custRow << custItem << new QStandardItem("");
            model->appendRow(custRow);

            QDomNodeList transactions = custElem.elementsByTagName("transaction");
            for (int j = 0; j < transactions.count(); ++j) {
                QString transText = transactions.at(j).toElement().text();
                QList<QStandardItem *> transRow;
                transRow << new QStandardItem("") << new QStandardItem(transText);
                custItem->appendRow(transRow);
            }
        }

        ui->treeView->expandAll();
    }
}
