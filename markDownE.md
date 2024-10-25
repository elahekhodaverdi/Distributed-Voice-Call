# Distributed Voice Call

## AudioInput Class

This class inherits from `QIODevice`, an abstract class in Qt that is used for handling device input and output. `QIODevice` provides a uniform interface for reading and writing data, as well as managing various data sources like buffers, files, sockets, etc. In `AudioInput`, we use it to write the data from `QAudioSource` after encoding.

### Fields

```cpp
    QAudioSource *audio;
    OpusEncoder *opusEncoder;
```

- **`audio`**: A pointer to the `QAudioSource`, used to capture audio from the input device.
- **`opusEncoder`**: A pointer to the Opus encoder, which is used to encode the raw audio data before writing it.

### Signals

```cpp
    void audioIsReady(const QByteArray &data);
```

This signal is emitted whenever a new chunk of encoded audio data is available.
 It sends the encoded audio as a QByteArray , which can be easily handled or sent across the network.

### Constructor

We initialize the audio source with a custom format and set up the encoder. The sample rate for both the encoder and the audio source is set to 48 kHz, as this is commonly supported by most audio hardware (e.g., microphones, sound cards, and my own setup).

```cpp
AudioInput::AudioInput()
{
    int error;
    opusEncoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_AUDIO, &error);
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    audio = new QAudioSource(format, this);
    ...
}
```

### `start()`

This method starts audio capture. To understand the overall process, it's important to know that `QAudioSource` has two `start` methods (overloads). One takes no arguments, and the other takes a pointer to an instance of `QIODevice`. When we use the latter, after capturing the audio, it passes the data to the `writeData` method of the `QIODevice` instance. In this case, since we pass a pointer to `AudioInput`, the captured audio data will be sent to the overridden `writeData` method in our class.

```cpp
void AudioInput::start()
{
    if (!this->open(QIODeviceBase::ReadWrite)) {
        qCritical() << "Failed to open QIODevice!";
        return;
    }
    audio->start(this);
}
```

### `stop()`

Stops the audio capture and closes the device.

### `writeData()`

As mentioned in the `start()` method, after capturing the audio, the data is passed to this method. Here, we encode the data using the Opus encoder and then emit the `audioIsReady` signal to send the encoded audio packet.

```cpp
qint64 AudioInput::writeData(const char *data, qint64 len)
{
    std::vector<unsigned char> opusData(960);
    int frameSize = len / 2;

    int encodedBytes = opus_encode(opusEncoder,
                                   reinterpret_cast<const opus_int16 *>(data),
                                   frameSize,
                                   opusData.data(),
                                   opusData.size());

    if (len < 0)
        return len;

    QByteArray encodedOpusData(reinterpret_cast<const char *>(opusData.data()), encodedBytes);
    Q_EMIT audioIsReady(encodedOpusData);
    return len;
}
```

We set the size of `opusData` to 960 because we are working with 20 ms frames. The sample rate (48 kHz) divided by 20 ms gives 960 (48000 / 50 = 960).

According to the Opus documentation, the input length for the `opus_encode` method is equal to `frame_size * channels * sizeof(opus_int16)`. Since the size of `opus_int16` is 2 bytes and we are using 1 channel, the frame size is equal to the length of the input data divided by 2 (`len / 2`).

### `readData()`

Due to inheriting from `QIODevice`, we must override this method. However, since we don't use it in this class, the implementation simply returns zero and doesn't perform any operation.
