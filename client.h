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
    ~Client() {}

Q_SIGNALS:
    void offerIsReadyToSend(const QString & id);
    void answerIsReadyToSend(const QString & id);

public Q_SLOTS:
    void sendOffer(const QString &id);
    void sendAnswer(const QString & id);
    void sendMessage(const QString &id);

private:
    sio::client client;
    QString mySocketId;
};

#endif // CLIENT_H
