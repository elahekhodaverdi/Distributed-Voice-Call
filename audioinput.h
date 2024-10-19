#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <QAudioSource>
#include <QIODevice>
#include <opus.h>

class AudioInput : public QIODevice
{
    Q_OBJECT
public:
    AudioInput();

private:
    void handleStateChanged(QAudio::State newState);
    QAudioSource *audio;
    OpusEncoder *opusEncoder;

protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
};

#endif // AUDIOINPUT_H