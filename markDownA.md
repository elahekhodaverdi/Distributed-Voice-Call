## AudioOutput Class

This class is responsible for handling audio output functionality. It inherits from `QObject` and uses Qt's audio framework along with the Opus decoder to play audio data. The class manages a queue of audio packets and provides mechanisms for decoding and playing them through the system's audio output device.

### Fields

```cpp
    OpusDecoder* decoder;
    std::queue<QByteArray> playQueue;
    QIODevice* ioDevice;
    QAudioFormat audioFormat;
    QAudioSink* audioSink;
    QMediaDevices mediaDevices;
    QMutex mutex;
```

- **`decoder`**: A pointer to the Opus decoder, used to decode the encoded audio data
- **`playQueue`**: A queue that stores incoming audio packets (QByteArray) waiting to be played
- **`ioDevice`**: A pointer to the QIODevice used for writing decoded audio data
- **`audioFormat`**: Defines the format of the audio output (sample rate, channels, etc.)
- **`audioSink`**: Handles the actual audio output to the system's audio device
- **`mediaDevices`**: Provides access to available media devices
- **`mutex`**: Ensures thread-safe access to the playQueue

### Signals

```cpp
    void newPacket();
```

This signal is emitted whenever new audio data is added to the playQueue, triggering the play mechanism.

### Constructor and Destructor

The constructor initializes the audio system and decoder, while the destructor cleans up the Opus decoder:

```cpp
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
```

### Setup Methods

#### `setupDecoder()`

Initializes the Opus decoder with a sample rate of 48 kHz and mono channel configuration:

```cpp
void AudioOutput::setupDecoder(){
    int error;
    int sampleRate = 48000;
    int channels = 1;

    decoder = opus_decoder_create(sampleRate, channels, &error);
    // Error handling...
}
```

#### `setupAudio()`

Configures the audio format and initializes the audio sink:

```cpp
void AudioOutput::setupAudio()
{
    audioFormat.setSampleRate(48000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleFormat(QAudioFormat::Int16);

    audioSink = new QAudioSink(QMediaDevices::defaultAudioOutput(), audioFormat, this);
    // Error handling...
}
```

### Core Functionality

#### `start()`

Initializes the audio output device and opens it for writing:

```cpp
void AudioOutput::start(){
    ioDevice = audioSink->start();
    ioDevice->open(QIODevice::WriteOnly);
    // Error handling...
}
```

#### `addData(const QByteArray &data)`

Adds new encoded audio data to the playQueue in a thread-safe manner:

```cpp
void AudioOutput::addData(const QByteArray &data){
    mutex.lock();
    playQueue.push(data);
    mutex.unlock();
    Q_EMIT newPacket();
}
```

#### `play()`

Decodes and plays the audio data from the queue. This method:
1. Takes the first packet from the queue
2. Decodes it using the Opus decoder
3. Writes the decoded data to the audio device

```cpp
void AudioOutput::play(){
    mutex.lock();
    QByteArray data = playQueue.front();

    std::vector<opus_int16> decodedOutput(960);

    int decodedBytes = opus_decode(decoder,
                                     reinterpret_cast<const unsigned char*>(data.data()),
                                     data.size(),
                                     decodedOutput.data(),
                                     960,
                                     0) * 2;

    const char* outputToWrite = reinterpret_cast<const char*>(decodedOutput.data());

    ioDevice->write(outputToWrite, decodedBytes);
    playQueue.pop();
    mutex.unlock();
}
```

The buffer size of 960 samples corresponds to 20ms of audio at 48 kHz sample rate, matching the same frame size used in the AudioInput class.

### State Management

#### `handleStateChanged(QAudio::State newState)`

Monitors the audio system's state changes and handles different states (Idle, Stopped, etc.):

```cpp
void AudioOutput::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
    case QAudio::IdleState:
        // Finished playing (no more data)
        break;
    case QAudio::StoppedState:
        // Stopped for other reasons
        break;
    default:
        // Other cases
        break;
    }
}
```

This implementation provides robust audio output functionality with proper synchronization and error handling, making it suitable for real-time audio playback applications.