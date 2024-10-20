#include "audiooutput.h"
#include <QDebug>

AudioOutput::AudioOutput(QObject *parent)
    : QObject{parent}
{
    setupAudio();
    setupDecoder();
    connect(this, &AudioOutput::newPacket, this, &AudioOutput::play);

}

AudioOutput::~AudioOutput(){
    opus_decoder_destroy(decoder);
}


void AudioOutput::setupDecoder(){
    int error;
    int sampleRate = 48000;
    int channels = 1;

    decoder = opus_decoder_create(sampleRate, channels, &error);

    if (error != OPUS_OK) {
        qFatal("Failed to create decoder.\n");
        return;
    }
}

void AudioOutput::setupAudio()
{
    audioFormat.setSampleRate(48000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleFormat(QAudioFormat::Int16);

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
        break;
    }
}

void AudioOutput::addData(const QByteArray &data){
    mutex.lock();
    playQueue.push(data);
    mutex.unlock();
    Q_EMIT newPacket();
}

void AudioOutput::play(){
    mutex.lock();
    QByteArray data = playQueue.front();
    qDebug() << "data size before decode: " << data.size();

    std::vector<opus_int16> decodedOutput(960);

    int decodedBytes = opus_decode(decoder,
                                     reinterpret_cast<const unsigned char*>(data.data()),
                                     data.size(),
                                     decodedOutput.data(),
                                     960,
                                     0) * 2;

    qDebug() << "data size after decode" << decodedBytes;
    const char* outputToWrite = reinterpret_cast<const char*>(decodedOutput.data());

    ioDevice->write(outputToWrite, decodedBytes);
    playQueue.pop();
    mutex.unlock();
}


