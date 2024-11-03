## WebRTC Class

The `WebRTC` class manages WebRTC peer-to-peer connections, allowing for the creation and management of audio data channels between peers. Using the `libdatachannel` library, it handles signaling, ICE candidate exchange, media track setup, and transmission of audio data. Key functionalities include SDP (Session Description Protocol) generation for offer and answer handling, configuration of audio tracks, and management of connection states with peers.

### Main Challenges

The main challenge in this part was sending the ICE candidates. When we sent SDPs immediately after generation without considering the ICE candidate gathering state, an error would occur while setting the remote candidate (sent via the server). So, we decided to send the SDP only when the ICE candidate gathering state is complete. As a result, we won’t send any ICE candidates before the gathering state is complete (We check it using `m_gatheringCompleted` variable).

Another problem was with generating the answer SDP. Initially, we used this:

```cpp
connection->setLocalDescription(rtc::Description::Type::Answer);
```

This caused exceptions and errors. We then found that after setting the offer SDP, WebRTC automatically handles generating answers, so we only need to use this syntax:

```cpp
connection->localDescription()->generateSdp();
```

Another problem was with the audio. The audio input and output were working correctly, but when we connected everything, the received audio was unclear and only produced noise. The issue was that we needed to remove the header (RTPHeader) after receiving it in the `readVariant` method.

```cpp
if (resultData.size())
    resultData.remove(0, sizeof(RtpHeader));
```


### Fields

Some of fields isn't used in our code so I igonred them in this section.

- **`m_sequenceNumber`**: Tracks the sequence number for RTP packets.
- **`m_gatheringCompleted`**: Boolean flag indicating if the ICE candidate gathering process has finished.
- **`m_bitRate`**: The current bit rate for the audio track, with a default of 48000.
- **`m_payloadType`**: Payload type identifier for RTP, defaulting to 111 (Opus).
- **`m_audio`**: Holds the audio configuration, including codecs and bit rates.
- **`m_ssrc`**: Synchronization source (SSRC) identifier for RTP.
- **`m_isOfferer`**: Indicates if the instance is in the "offer" role in the connection.
- **`m_localId`**: Local peer identifier.
- **`m_config`**: Stores ICE server and other WebRTC connection configurations.
- **`m_peerConnections`**: Maps peer identifiers to their peer connection objects.
- **`m_peerTracks`**: Maps peer identifiers to their audio tracks.
- **`m_localDescription`** and **m_remoteDescription**: Store local and remote SDP descriptions.

### Signals

- **`connectionClosed`**: Emitted when a connection to a peer is closed.
- **`incommingPacket`**: Emitted when a new packet arrives from a peer.
- **`localDescriptionGenerated`**: Signals that a local SDP description is ready.
- **`localCandidateGenerated`**: Signals a generated ICE candidate for a peer.
- **`gatheringCompleted`**, **offerIsReady**, **answerIsReady**: Emit notifications for offer/answer readiness and gathering completion.
- **`ssrcChanged`**, **payloadTypeChanged**, **bitRateChanged**: Notify listeners of changes to SSRC, payload type, and bit rate.
- **`rtcConnected`**: Emitted when a WebRTC connection has successfully been established.

### Methods

I only mention the important methods, getters and setters(or resetters) aren't included in this section.

- **`init`**: Initializes WebRTC settings, including ICE servers and audio configuration.
- **`addPeer`**: Adds a new peer connection and sets up callback functions for handling peer events.
- **`generateOfferSDP`** / **generateAnswerSDP**: Create and set local offer or answer SDP, setting the offerer role accordingly.
- **`addAudioTrack`**: Adds an audio track to a peer connection, enabling packet transmission and handling incoming audio data.
- **`sendTrack`**: Sends encoded audio data as RTP packets to a peer.
- **`setRemoteDescription`**: Sets remote SDP information for a peer connection.
- **`setRemoteCandidate`**: Adds an ICE candidate for NAT traversal.
- **`readVariant`**: Converts a `rtc::message_variant` into a QByteArray.
- **`descriptionToJson`**: Converts SDP description objects to JSON.
- **`removeConnectionData`**: Cleans up peer-specific data when a connection is closed.
- **`closeConnection`**: Responsible for closing the connection of a specific peer.

### **`Constructor`**

Initializes a new instance and establishes a connection for the `gatheringCompleted` signal. Once `gatheringCompleted` is emitted, the instance will trigger either the `offerIsReady` or `answerIsReady` signal, depending on the user’s role.

```cpp
    connect(this, &WebRTC::gatheringCompleted, [this] (const QString &peerId) {
        if (!m_gatheringCompleted) return;
        m_localDescription = descriptionToJson(m_peerConnections[peerId]->localDescription().value());
        Q_EMIT localDescriptionGenerated(peerId, m_localDescription);
        if (m_isOfferer)
            Q_EMIT this->offerIsReady(peerId, m_localDescription);
        else
            Q_EMIT this->answerIsReady(peerId, m_localDescription);
    });
```

### **`init(const QString &id, bool isOfferer = false)`**

Initializes WebRTC with specified peer ID and role (offerer or not). Configures audio track settings and sets up ICE servers for STUN/TURN communication.

### **`addPeer(const QString &peerId)`**

Adds a new peer connection with specified ID. Configures callbacks for SDP generation, ICE candidate handling, connection state, and gathering state.

If the ICE Candidates were generated completely we update m_localDescription and m_isOfferer based on new generated local description.

```cpp
    // Set up a callback for when the local description is generated
    newPeer->onLocalDescription([this, peerId](const rtc::Description &description) {
        if (m_gatheringCompleted) return;
        auto typeString = QString::fromStdString(description.typeString());
        auto sdp = QString::fromStdString(description);
        m_isOfferer = (typeString == "offer");
        m_localDescription = descriptionToJson(description);

    });
```

 When an ICE candidate becomes available, the `localCandidateGenerated` signal is emitted, depending on the gathering state. If ICE candidates have already been completely gathered, `localCandidateGenerated` is emitted to send the candidate to the other client via the signaling server.

```cpp
    // Set up a callback for handling local ICE candidates
    newPeer->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        if (!m_gatheringCompleted) return;
        Q_EMIT localCandidateGenerated(peerId,
                                     QString::fromStdString(candidate.candidate()),
                                     QString::fromStdString(candidate.mid()));
    });
```

Upon a change in connection state, if the new state is "connected," the relevant signal is emitted. If the connection has been closed, connection data is first removed from the maps, `m_gatheringCompleted` is reset, and then the corresponding signal is emitted.

```cpp
    newPeer->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        // Handle different states like New, Connecting, Connected, Disconnected, etc.
        switch (state) {
        // Ignore remaining states
        case rtc::PeerConnection::State::Connected:
            Q_EMIT rtcConnected();
            break;
        case rtc::PeerConnection::State::Closed:
            removeConnectionData(peerId);
            Q_EMIT connectionClosed();
            break;
        default:
            break;
        }
    });
```

Sets up a callback to track the gathering state of the peer connection. When the gathering completes, the gatheringCompleted signal is emitted with the peer ID to indicate readiness for communication.

```cpp
    newPeer->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        // When the gathering is complete, emit the gatheringComplited signal
        if (rtc::PeerConnection::GatheringState::Complete == state) {
            m_gatheringCompleted = true;
            Q_EMIT gatheringCompleted(peerId);
        }
    });
```

Sets up a callback to handle incoming media tracks. When a track is received, it is stored in `m_peerTracks` for the specified peer ID. An `onMessage` callback is also included, though it won't be used here since the correct callback will be set in the `addAudioTrack` method. This structure follows the provided template.

```cpp
    newPeer->onTrack([this, peerId](std::shared_ptr<rtc::Track> track) {
        // handle the incoming media stream, emitting the incommingPacket signal if a stream is received
        m_peerTracks[peerId] = track;
        track->onMessage([this, peerId](rtc::message_variant data) {
            qDebug() << "on message called in add peer";
        });
    });
```

### **`generateOfferSDP(const QString &peerId)`** and **`generateAnswerSDP(const QString &peerId)`**

Initiate the SDP offer or answer generation process, setting the local SDP role accordingly.
If the peer doesn't exist already, They will add it at first.

### **`addAudioTrack(const QString &peerId, const QString &trackName)`**

Adds an audio track to the specified peer connection, setting up message handling for audio data. Emits `incommingPacket` when new audio packets are received.

### **`sendTrack(const QString &peerId, const QByteArray &buffer)`**

Encodes audio data into RTP packets with a constructed RTP header and sends it to the peer. The packet includes details like SSRC, sequence number, and timestamp.

### **`setRemoteDescription(const QString &peerId, const QString &sdp)`**

Sets the remote SDP for a peer connection based on provided session data, initializing the connection from the remote side.

### **`setRemoteCandidate(const QString &peerId, const QString &candidate, const QString &sdpMid)`**

Adds remote ICE candidates to facilitate NAT traversal.

### **`readVariant(const rtc::message_variant &data)`**

A helper method that reads `rtc::message_variant` data and converts it into a `QByteArray`. After conversion, it removes the `RtpHeader` added when sending data in the `sendTrack` method:

```cpp
    if (!resultData.isEmpty())
        resultData.remove(0, sizeof(RtpHeader));
```

### **`descriptionToJson(const rtc::Description &description)`**

Converts SDP descriptions to JSON format, simplifying SDP handling between peers.

The structure of Json is:

```json
{
    "type": "offer", // or answer
    "sdp":  "<sdp>"
}
```

### **`removeConnectionData(const QString &peerId)`**

Removes peer-specific data when a connection is closed, resetting relevant internal states.

### **`closeConnection(const QString &peerId)`**

This method is responsible for terminating a WebRTC connection associated with a specific peer ID. It checks if the peer ID exists in the `m_peerConnections` map. If it does, it calls the `close()` method on the corresponding connection object, effectively ending the connection. After closing the connection, it invokes the `removeConnectionData` method to clean up any associated data related to that peer ID.
