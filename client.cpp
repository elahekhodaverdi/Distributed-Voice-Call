#include "client.h"
#include <QDebug>
#include <QTextStream>

Client::Client(QObject *parent)
    : QObject(parent)
{
    client.connect("http://localhost:3000");

    client.socket()->on("your_id", sio::socket::event_listener([this](sio::event &ev) {
                            QString data = QString::fromStdString(ev.get_message()->get_string());
                            mySocketId = data;
                        }));
    client.socket()->on("receive_message", sio::socket::event_listener([this](sio::event &ev) {
                            auto data = ev.get_message()->get_map();
                            QString fromClientId = QString::fromStdString(
                                data["from"]->get_string());
                            QString message = QString::fromStdString(data["message"]->get_string());
                            qDebug() << "Message from client" << fromClientId << ":" << message;
                        }));

    connect(&inputThread, &QThread::started, this, &Client::sendMessage);
    inputThread.start();
}

void Client::sendMessage()
{
    QTextStream stream(stdin);
    QString targetClientId, message;

    qDebug() << "Enter target client ID:";
    stream >> targetClientId;

    qDebug() << "Enter message:";
    stream >> message;

    sio::message::ptr msgData = sio::object_message::create();
    msgData->get_map()["targetClientId"] = sio::string_message::create(targetClientId.toStdString());
    msgData->get_map()["message"] = sio::string_message::create(message.toStdString());
    client.socket()->emit("send_message", sio::message::list(msgData));
}
