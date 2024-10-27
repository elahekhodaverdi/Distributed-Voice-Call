#ifndef CLIENT_H
#define CLIENT_H

#include <SocketIO/sio_client.h>
#include <QObject>

class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString newSdp READ newSdp NOTIFY newSdpReceived)

public:
    explicit Client(QObject *parent = nullptr);

    QString mySocketId() const { return m_mySocketId; }
    QString newSdp() const { return m_newSdp; }

Q_SIGNALS:
    void newSdpReceived(const QString &peerID, const QString &sdp);
    void answerIsReadyToGenerate(const QString &id);
    void localIdIsSet(const QString &id, bool is_offerer);
    void newIceCandidateReceived(const QString &id, const QString &candidate, const QString &mid);
public Q_SLOTS:
    void sendIceCandidate(const QString &id, const QString &candidate, const QString &mid);
    void sendOffer(const QString &id, const QString &sdp);
    void sendAnswer(const QString &id, const QString &sdp);

private:
    QString m_mySocketId;
    QString m_newSdp;
    sio::client client;
};

#endif // CLIENT_H
