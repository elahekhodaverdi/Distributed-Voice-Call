#include "audioinput.h"
#include <QAudioFormat>
#include <QDebug>
#include <QMediaDevices>

// #include <opus
AudioInput::AudioInput()
{
    int error;
    opusEncoder = opus_encoder_create(8000, 1, OPUS_APPLICATION_AUDIO, &error);
    QAudioFormat format;
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    audio = new QAudioSource(format, this);
    if (!audio) {
        qCritical() << "Failed to initialize audio source!";
        return;
    }
    connect(audio, &QAudioSource::stateChanged, this, &AudioInput::handleStateChanged);
    if (!this->open(QIODeviceBase::ReadWrite)) {
        qCritical() << "Failed to open QIODevice!";
        return;
    }
    audio->start(this);
}

AudioInput::~AudioInput()
{
    opus_encoder_destroy(opusEncoder);
}
void AudioInput::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
    case QAudio::ActiveState:
        qDebug() << "Audio is active";
        break;
    case QAudio::SuspendedState:
        qDebug() << "Audio is suspended";
        break;
    case QAudio::StoppedState:
        qDebug() << "Audio is stopped";
        break;
    case QAudio::IdleState:
        qDebug() << "Audio is idle";
        break;
    default:
        break;
    }
}

qint64 AudioInput::writeData(const char *data, qint64 len)
{
    unsigned char opusData[4000];
    int frameSize = len / sizeof(opus_int16);

    int encodedBytes = opus_encode(opusEncoder,
                                   reinterpret_cast<const opus_int16 *>(data),
                                   frameSize,
                                   opusData,
                                   sizeof(opusData));
    if (len < 0) {
        return len;
    }

    QByteArray encodedOpusData(reinterpret_cast<const char *>(opusData), encodedBytes);
    Q_EMIT AudioIsReady(encodedOpusData);
    return len;
}

qint64 AudioInput::readData(char *data, qint64 maxlen)
{
    return 0;
}
