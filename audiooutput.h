#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>
#include <QIODevice>
#include <QAudioFormat>
#include <QAudioSink>
#include <QMediaDevices>
#include <QMutex>
#include <QBuffer>
#include <queue>
//#include <opus.h>

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioOutput(QObject *parent = nullptr);
    ~AudioOutput();

public slots:
    void addData(const QByteArray &data);
    void play();

private:
    void setupAudio();
    void setupDecoder();
    //OpusDecoder* decoder;
    std::queue<QByteArray> playQueue;
    QIODevice* ioDevice;
    QAudioFormat audioFormat;
    QAudioSink* audioSink;
    QMediaDevices mediaDevices;
    QMutex mutex;

signals:
    void newPacket();  // Signal emitted when new data is added

};

#endif // AUDIOOUTPUT_H
