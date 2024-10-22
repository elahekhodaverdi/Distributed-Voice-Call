#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "audioinput.h"
#include "audiooutput.h"
#include "client.h"
#include <webrtc.h>
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    // AudioInput audioInput;
    // AudioOutput audioOutput;
    Client client;
    WebRTC webrtc;
    // QObject::connect(&audioInput, &AudioInput::AudioIsReady, &audioOutput, &AudioOutput::addData);

    QObject::connect(&client, &Client::localIdIsSet, &webrtc, &WebRTC::init);
    QObject::connect(&webrtc, &WebRTC::offerIsReady, &client, &Client::sendOffer);
    QObject::connect(&webrtc, &WebRTC::answerIsReady, &client, &Client::sendAnswer);
    QObject::connect(&client, &Client::newSdpReceived, &webrtc, &WebRTC::setRemoteDescription);
    QObject::connect(&client, &Client::newIceCandidateReceived, &webrtc, &WebRTC::setRemoteCandidate);
    QObject::connect(&webrtc, &WebRTC::localCandidateGenerated, &client, &Client::sendIceCandidate);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("client", &client);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
