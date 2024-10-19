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

Q_SIGNALS:
    void offerIsReadyToSend(QString id);
    void answerIsReadyToSend(QString id);

public Q_SLOTS:
    void sendOffer(QString id);
    void sendAnswer(QString id);
    void sendMessage();

private:
    sio::client client;
    QString mySocketId;
    QThread inputThread;
};

#endif // CLIENT_H
