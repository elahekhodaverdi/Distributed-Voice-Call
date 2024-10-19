#ifndef CLIENT_H
#define CLIENT_H

#include "SocketIO/sio_client.h"
#include <QObject>
#include <QString>
#include <QThread>

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);

public Q_SLOTS:
    void sendMessage();

private:
    sio::client client;
    QString mySocketId;
    QThread inputThread;
};

#endif // CLIENT_H
