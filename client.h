#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QString>
#include <QThread>
#include "SocketIO/sio_client.h"

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);

private slots:
    void sendMessage();

private:
    sio::client client;
    QString mySocketId;
    QThread inputThread;
};

#endif // CLIENT_H
