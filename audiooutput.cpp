#include "audiooutput.h"
#include <QDebug>

AudioOutput::AudioOutput(QObject *parent)
    : QObject{parent}
{
    setupAudio();
    connect(this, &AudioOutput::newPacket, this, &AudioOutput::play);
}

AudioOutput::~AudioOutput(){
    //opus_decoder_destroy(decoder);
}


// void AudioOutput::setupDecoder(){
//     int error;
//     int sampleRate = 8000;
//     int channels = 1;

//     decoder = opus_decoder_create(sampleRate, channels, &error);

//     if (error != OPUS_OK) {
//         qFatal("Failed to create decoder.\n");
//         return;
//     }
// }

void AudioOutput::setupAudio()
{
    audioFormat.setSampleRate(48000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleFormat(QAudioFormat::Float);
    if (!audioFormat.isValid()) {
        qDebug() << "audio format is not valid";
        return;
    }
    audioSink = new QAudioSink(QMediaDevices::defaultAudioOutput(), audioFormat, this);
    if (!audioSink) {
        qCritical() << "Failed to initialize audio sink!";
        return;
    }
    connect(audioSink, &QAudioSink::stateChanged, this, &AudioOutput::handleStateChanged);
    ioDevice = audioSink->start();
    if (!ioDevice) {
        qCritical() << "Failed to start audio output!";
        return;
    }
}

void AudioOutput::handleStateChanged(QAudio::State newState)
{
    qDebug() << "here in state change!\n";
    switch (newState) {
    case QAudio::IdleState:
        // Finished playing (no more data)
        qDebug() << "idle\n";
        break;

    case QAudio::StoppedState:
        // Stopped for other reasons
        // if (->error() != QAudio::NoError) {
        //     // Error handling
        // }
        break;

    default:
        // ... other cases as appropriate
        qDebug() << "new" << newState;
        break;
    }
}

void AudioOutput::addData(const QByteArray &data){
    mutex.lock();
    qDebug() << "hehe6";
    playQueue.append(data.data());
    mutex.unlock();
    emit newPacket();
}

void AudioOutput::play(){
    mutex.lock();
    long long audiolen = std::min(playQueue.size(), audioSink->bytesFree());
    QByteArray data = playQueue.first(audiolen);
    playQueue.remove(0, audiolen);
    // opus_int16 decodedOutput[160];

    // int decodedBytes = opus_decode(decoder,
    //                                  reinterpret_cast<const unsigned char*>(data.constData()),
    //                                  data.size(),
    //                                  decodedOutput,
    //                                  160,
    //                                  0);
    // const char* outputToWrite = reinterpret_cast<const char*>(decodedOutput);
    qDebug() << playQueue.size() << "," << data.size() << " , " << audioSink->bytesFree();
    int a = ioDevice->write(data.data(), audiolen);
    qDebug() << "hehe9: " << a;

    mutex.unlock();
}


