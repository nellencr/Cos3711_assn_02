#include "senderthread.h"
#include <QDomDocument>
#include <QStandardItem>
#include <QUdpSocket>
#include <QHostAddress>


SenderThread::SenderThread(QStandardItemModel *model, QObject *parent)
    : QThread(parent), model(model) {}

QString SenderThread::generateXML() {
    QDomDocument doc;
    QDomElement root = doc.createElement("Store");
    doc.appendChild(root);

    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *custItem = model->item(i);
        QDomElement custElem = doc.createElement("Customer");
        custElem.setAttribute("name", custItem->text());

        for (int j = 0; j < custItem->rowCount(); ++j) {
            QStandardItem *transItem = custItem->child(j, 1);
            if (transItem) {
                QDomElement transElem = doc.createElement("Transaction");
                QDomText text = doc.createTextNode(transItem->text());
                transElem.appendChild(text);
                custElem.appendChild(transElem);
            }
        }

        root.appendChild(custElem);
    }

    return doc.toString();
}

void SenderThread::run() {
    QString xmlData = generateXML();
    QByteArray datagram = xmlData.toUtf8();

    QUdpSocket socket;
    QHostAddress address = QHostAddress::Broadcast;
    quint16 port = 45454;

    socket.writeDatagram(datagram, address, port);
}

