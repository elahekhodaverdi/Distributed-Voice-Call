#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "audio/audioinput.h"
#include "audio/audiooutput.h"
#include "network/client.h"
#include "network/webrtc.h"
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<WebRTC>("Webrtc", 1, 0, "WebRTC");
    qmlRegisterType<Client>("Client", 1, 0, "Client");
    qmlRegisterType<AudioInput>("Audio", 1, 0, "AudioInput");
    qmlRegisterType<AudioOutput>("Audio", 1, 0, "AudioOutput");

    const QUrl url(QStringLiteral("qrc:/src/UI/main.qml"));
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
