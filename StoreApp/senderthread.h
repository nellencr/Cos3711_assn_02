#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QThread>
#include <QStandardItemModel>

class SenderThread : public QThread {
    Q_OBJECT

public:
    explicit SenderThread(QStandardItemModel *model, QObject *parent = nullptr);
    void run() override;

private:
    QStandardItemModel *model;
    QString generateXML();
};

#endif // SENDERTHREAD_H

