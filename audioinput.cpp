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
    format.setSampleFormat(QAudioFormat::UInt8);

    QAudioDevice device = QMediaDevices::defaultAudioInput();

    if (!device.isFormatSupported(format)) {
        qDebug() << "format not supported";
        format = device.preferredFormat();
    }

    audio = new QAudioSource(device, format, this);

    connect(audio, &QAudioSource::stateChanged, this, &AudioInput::handleStateChanged);
    this->open(QIODeviceBase::ReadWrite);
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

    if (encodedBytes < 0)
        qDebug() << "ridi";
    QByteArray encodedOpusData(reinterpret_cast<const char *>(opusData), encodedBytes);
    emit AudioIsReady(encodedOpusData);
    return encodedBytes;
}

qint64 AudioInput::readData(char *data, qint64 maxlen)
{
    qDebug() << "ridi2";
    return 0;
}
