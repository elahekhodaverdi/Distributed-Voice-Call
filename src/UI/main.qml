import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import Webrtc
import Client
import Audio

Window {
    width: 280
    height: 520
    visible: true
    title: qsTr("CA1")

    WebRTC {
        id: webrtc

        onOfferIsReady: (peerId, description) => client.sendOffer(peerId, description);

        onAnswerIsReady: (peerId, description) => client.sendAnswer(peerId, description);

        onLocalCandidateGenerated: (id, candidate, mid) => client.sendIceCandidate(id, candidate, mid);

        onIncommingPacket: (id, data, len) => output.addData(data);

        onRtcConnected: () => {
                            callbtn.pushed = true;
                            callbtn.Material.background = "red"
                            callbtn.text = "End Call"
                            input.start();
                            output.start();
                        }

        onConnectionClosed: () => {
                                callbtn.pushed = false;
                                callbtn.Material.background = "green"
                                callbtn.text = "Call"
                                input.stop()
                                output.stop();
                                textfield.clear()
                            }

    }

    Client{
        id: client

        onLocalIdIsSet: (id, isOfferer) => {
                            webrtc.init(id, isOfferer);
                            myIdText.text = "My ID: " + id;
                        }

        onNewSdpReceived: (id, description) => {
                              webrtc.addPeer(id);
                              webrtc.setRemoteDescription(id, description)
                          };

        onNewIceCandidateReceived: (id, candidate, mid) => webrtc.setRemoteCandidate(id, candidate, mid)

        onAnswerIsReadyToGenerate: (id) => {
                                   textfield.text = id;
                                   webrtc.generateAnswerSDP(id);
                                   }
    }

    AudioOutput {
        id: output

    }

    AudioInput{
        id: input
        onAudioIsReady: (data) => webrtc.sendTrack(textfield.text, data);

    }

    Item {
        anchors.fill: parent

        ColumnLayout {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: textfield.top
                margins: 20
            }

            TextEdit {
                text: "Ip: " + "172.16.142.176"
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                readOnly: true
                selectByMouse: true
                font.pixelSize: 12 // Adjust the size as needed
            }
            TextEdit {
                text: "IceCandidate: " + "172.16.142.176"
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                readOnly: true
                selectByMouse: true
                font.pixelSize: 12 // Adjust the size as needed
            }
            TextEdit {
                text: "CallerId: " + textfield.text
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                readOnly: true
                selectByMouse: true
                font.pixelSize: 12 // Adjust the size as needed
            }
            TextEdit {
                id: myIdText
                text: "My ID: "
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                readOnly: true
                selectByMouse: true
                font.pixelSize: 12 // Adjust the size as needed
            }
        }

        TextField {
            id: textfield
            placeholderText: "Phone Number"
            anchors.bottom: callbtn.top
            anchors.bottomMargin: 10
            anchors.left: callbtn.left
            anchors.right: callbtn.right
            enabled: !callbtn.pushed
        }

        Button {
            id: callbtn

            property bool pushed: false

            height: 47
            text: "Call"
            Material.background: "green"
            Material.foreground: "white"
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: 20
            }

            onClicked: {
                pushed = !pushed
                if (pushed) {
                    Material.background = "red"
                    text = "End Call"
                    webrtc.addPeer(textfield.text)
                    webrtc.generateOfferSDP(textfield.text)
                } else {
                    webrtc.closeConnection(textfield.text);
                }
            }
        }
    }
}
