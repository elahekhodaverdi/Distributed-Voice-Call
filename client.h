#ifndef CLIENT_H
#define CLIENT_H

#include "SocketIO/sio_client.h"
#include <QObject>

class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString mySocketId READ mySocketId NOTIFY mySocketIdChanged)

public:
    explicit Client(QObject *parent = nullptr);

    QString mySocketId() const { return m_mySocketId; }

Q_SIGNALS:
    void mySocketIdChanged();
    void offerIsReadyToSend(const QString &id);
    void answerIsReadyToSend(const QString &id);

public Q_SLOTS:
    void sendMessage(const QString &id);
    void sendOffer(const QString &id);
    void sendAnswer(const QString &id);

private:
    QString m_mySocketId;
    sio::client client;
};

#endif // CLIENT_H
