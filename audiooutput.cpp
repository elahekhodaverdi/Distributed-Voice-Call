#include "audiooutput.h"

AudioOutput::AudioOutput(QObject *parent)
    : QObject{parent}
{
    connect(this, &AudioOutput::newPacket, this, &AudioOutput::play);
}

void AudioOutput::setupAudio()
{
    audioFormat.setSampleRate(8000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleFormat(QAudioFormat::UInt8);

    audioSink = new QAudioSink(QMediaDevices::defaultAudioOutput(), audioFormat, this);
    ioDevice = audioSink->start();
}

void AudioOutput::addData(const QByteArray &data){
    mutex.lock();
    playQueue.push(data);
    emit newPacket();
    mutex.unlock();
}

void AudioOutput::play(){
    mutex.lock();
    int error;
    int sampleRate = 8000;
    int channels = 1;

    OpusDecoder* decoder = opus_decoder_create(sampleRate, channels, &error);

    if (error != OPUS_OK) {
        qFatal("Failed to create decoder.\n");
        return;
    }


    QByteArray data = playQueue.front();
    playQueue.pop();
    opus_int16 decodedOutput[160];

    int decodedBytes = opus_decode(decoder,
                                     reinterpret_cast<const unsigned char*>(data.constData()),
                                     data.size(),
                                     decodedOutput,
                                     160,
                                     0);
    const char* outputToWrite = reinterpret_cast<const char*>(decodedOutput);

    ioDevice->write(outputToWrite);


    mutex.unlock();
}


