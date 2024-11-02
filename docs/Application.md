# Application Overview

This application sets up a basic real-time communication interface using QML and C++. It leverages the `WebRTC`, `Client`, `AudioInput`, and `AudioOutput` classes to enable audio communication over TCP, with WebRTC handling signaling. The main QML file provides a user interface for initiating and managing audio calls, while `main.cpp` registers the classes with the QML engine and loads the QML interface.

## Main Challenges

The main challenge in bringing all the parts together was interacting with the UI. We needed to call class methods based on UI actions, synchronize with signals from other classes, and retrieve some data from the UI. For class signaling, we could have used `QObject::connect`, but since some UI components needed to be updated and data retrieved based on triggered signals, we used `qmlRegisterType` instead. For example:

```cpp
qmlRegisterType<WebRTC>("Webrtc", 1, 0, "WebRTC");
```

## **`main.cpp`**

The `main.cpp` file creates a `QGuiApplication` instance, registers custom C++ classes for use in QML, and loads the QML user interface.

The following lines register the C++ classes as QML types so they can be accessed in QML:

```cpp
qmlRegisterType<WebRTC>("Webrtc", 1, 0, "WebRTC");
qmlRegisterType<Client>("Client", 1, 0, "Client");
qmlRegisterType<AudioInput>("Audio", 1, 0, "AudioInput");
qmlRegisterType<AudioOutput>("Audio", 1, 0, "AudioOutput");
```

Explanation of each line:

- **`qmlRegisterType<Class>("Module", MajorVersion, MinorVersion, "TypeName")`**:
    - Registers the C++ class (`WebRTC`, `Client`, `AudioInput`, `AudioOutput`) for use in QML.
    - **Module**: Groups related types (e.g., `"Webrtc"`, `"Client"`, `"Audio"`).
    - **Version**: Specifies the module version (`1.0`).
    - **TypeName**: The name used in QML (e.g., `WebRTC`).

## **`main.qml`**

In the QML file, the following components registered in `main.cpp` are used:

### **WebRTC**

```qml
WebRTC {
    id: webrtc
    // Additional properties and logic
}
```

### **Client**

```qml
Client {
    id: client
    // Additional properties and logic
}
```

### **AudioOutput**

```qml
AudioOutput {
    id: output
    // Additional properties and logic
}
```

### **AudioInput**

```qml
AudioInput {
    id: input
    // Additional properties and logic
}
```

## Application Flow

To understand the workflow, let's follow the sequence of operations:

1. **Initialization**:

   In the `Client` class, when the program starts, the server sends a unique ID. Upon receiving this ID, the `localIdIsSet` signal is emitted. This triggers the initialization of the `WebRTC` instance and updates the UI.

   ```qml
   onLocalIdIsSet: (id, isOfferer) => {
       webrtc.init(id, isOfferer);
       myIdText.text = "My ID: " + id;
   }
   ```

2. **Initiating a Call**:

    When client A wants to call client B, the ID of client B is entered in the `TextField` component, and the call button is clicked. The `onClicked` handler updates the button state and initiates the call:

   ```qml
   Material.background = "red"
   text = "End Call"
   webrtc.addPeer(textfield.text)
   webrtc.generateOfferSDP(textfield.text)
   ```

   The `generateOfferSDP` method in `WebRTC` emits the `onOfferIsReady` signal when the offer is prepared. This offer is sent to the target client via the signaling server:

   ```qml
   onOfferIsReady: (peerId, description) => client.sendOffer(peerId, description);
   ```

3. **Receiving an Offer**:

   Client B receives the offer through the server. The `Client` class emits the `newSdpReceived` and `answerIsReadyToGenerate` signals, triggering:

   ```qml
   onNewIceCandidateReceived: (id, candidate, mid) => webrtc.setRemoteCandidate(id, candidate, mid)

   onAnswerIsReadyToGenerate: (id) => {
       textfield.text = id;
       webrtc.generateAnswerSDP(id);
   }
   ```

   After generating the answer, the `onAnswerIsReady` signal is emitted, and the answer is sent back via the signaling server:

   ```qml
   onAnswerIsReady: (peerId, description) => client.sendAnswer(peerId, description);
   ```

4. **Establishing the Connection**:

   Once Client A receives the answer, it sets the remote description and waits for the connection to be established. The `rtcConnected` signal is emitted upon successful connection in both clients:

   ```qml
   onRtcConnected: () => {
       callbtn.pushed = true;
       callbtn.Material.background = "red"
       callbtn.text = "End Call"
       input.start();
       output.start();
   }
   ```

5. **Audio Transmission**:

   Audio packets are encoded and sent through tracks using WebRTC. When an audio packet is ready, the `audioIsReady` signal triggers its transmission:

   ```qml
   onAudioIsReady: (data) => webrtc.sendTrack(textfield.text, data);
   ```

   On the receiving side, the `incomingPacket` signal processes the packet, sending the data buffer to `AudioOutput` for playback:

   ```qml
   onIncomingPacket: (id, data, len) => output.addData(data);
   ```

6. **Ending the Call**:

    Clicking the "End Call" button closes the connection, resets the UI, and stops the audio components:

   ```qml
   webrtc.closeConnection(textfield.text);
   ```

   The `connectionClosed` signal is emitted, and the application cleans up accordingly:

   ```qml
   onConnectionClosed: () => {
       input.stop();
       output.stop();
       callbtn.pushed = false;
       callbtn.Material.background = "green"
       callbtn.text = "Call"
   }
   ```
