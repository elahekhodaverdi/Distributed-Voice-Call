#include "client.h"
#include <QDebug>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

Client::Client(QObject *parent)
    : QObject(parent)
{

    client.socket()->on("your_id", sio::socket::event_listener([this](sio::event &ev) {
                            QString data = QString::fromStdString(ev.get_message()->get_string());
                            qDebug() << "MY ID is:  " << data;
                            mySocketId = data;
                        }));
    client.socket()->on("offer_sdp", sio::socket::event_listener([this](sio::event &ev) {
                            auto data = ev.get_message()->get_map();
                            QString fromClientId = QString::fromStdString(
                                data["from"]->get_string());
                            QString sdp = QString::fromStdString(data["sdp"]->get_string());
                            qDebug() << "Message from client" << fromClientId << ":" << sdp;
                            Q_EMIT answerIsReadyToSend(fromClientId);
                        }));
    client.socket()->on("answer_sdp", sio::socket::event_listener([this](sio::event &ev) {
                            auto data = ev.get_message()->get_map();
                            QString fromClientId = QString::fromStdString(
                                data["from"]->get_string());
                            QString message = QString::fromStdString(data["sdp"]->get_string());
                            qDebug() << "Message from client" << fromClientId << ":" << message;
                        }));

    client.connect("http://localhost:3000");
    connect(this, &Client::answerIsReadyToSend, this, &Client::sendAnswer);
    connect(this, &Client::offerIsReadyToSend, this, &Client::sendOffer);
}

void Client::sendMessage(const QString &id)
{
    qDebug() << "send message";
    Q_EMIT offerIsReadyToSend(id);
}

void Client::sendOffer(const QString & id)
{
    qDebug() << "send Offer";
    QJsonObject msg;
    msg["targetClientId"] = id;
    msg["sdp"] = "elahe";
    std::string sdpJson = QString(QJsonDocument(msg).toJson(QJsonDocument::Compact)).toStdString();
    client.socket()->emit("offer_sdp", sio::message::list(sdpJson));
}

void Client::sendAnswer(const QString & id)
{
    qDebug() << "send Answer";
    QJsonObject msg;
    msg["targetClientId"] = id;
    msg["sdp"] = "ali";
    std::string sdpJson = QString(QJsonDocument(msg).toJson(QJsonDocument::Compact)).toStdString();
    client.socket()->emit("answer_sdp", sio::message::list(sdpJson));
}
