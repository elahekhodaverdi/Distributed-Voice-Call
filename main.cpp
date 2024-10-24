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
    //Client client;
    //WebRTC webrtc;
    // QObject::connect(&audioInput, &AudioInput::AudioIsReady, &audioOutput, &AudioOutput::addData);

    // QObject::connect(&client, &Client::localIdIsSet, &webrtc, &WebRTC::init);
    // QObject::connect(&client, &Client::newSdpReceived, &webrtc, &WebRTC::setRemoteDescription);
    // QObject::connect(&client, &Client::newIceCandidateReceived, &webrtc, &WebRTC::setRemoteCandidate);

    // QObject::connect(&webrtc, &WebRTC::offerIsReady, &client, &Client::sendOffer);
    // QObject::connect(&webrtc, &WebRTC::answerIsReady, &client, &Client::sendAnswer);
    // QObject::connect(&webrtc, &WebRTC::localCandidateGenerated, &client, &Client::sendIceCandidate);

    QQmlApplicationEngine engine;
    qmlRegisterType<WebRTC>("Webrtc", 1, 0, "WebRTC");
    qmlRegisterType<Client>("Client", 1, 0, "Client");
    qmlRegisterType<AudioInput>("Audio", 1, 0, "AudioInput");
    qmlRegisterType<AudioOutput>("Audio", 1, 0, "AudioOutput");

    // engine.rootContext()->setContextProperty("audioInput", &audioInput);
    //engine.rootContext()->setContextProperty("client", &client);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
            if (obj)
                qDebug() << "QML component loaded successfully";
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
