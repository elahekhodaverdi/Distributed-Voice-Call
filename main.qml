import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import webrtc
Window {
    width: 280
    height: 520
    visible: true
    title: qsTr("CA1")
    WebRTC {
        id: webrtc
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
                text: "My ID: " + client.mySocketId
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                readOnly: true
                selectByMouse: true
                font.pixelSize: 12 // Adjust the size as needed
            }
            TextEdit {
                text: "Latest sdp: " + client.newSdp
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
                    // audioInput.start()
                    client.sendMessage(textfield.text)
                } else {
                    Material.background = "green"
                    text = "Call"
                    // audioInput.stop()
                    textfield.clear()
                }
            }
        }
    }
}
