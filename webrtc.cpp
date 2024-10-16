#include "webrtc.h"
#include <QtEndian>
#include <QJsonDocument>

static_assert(true);

#pragma pack(push, 1)
struct RtpHeader {
    uint8_t first;
    uint8_t marker:1;
    uint8_t payloadType:7;
    uint16_t sequenceNumber;
    uint32_t timestamp;
    uint32_t ssrc;
};
#pragma pack(pop)


WebRTC::WebRTC(QObject *parent)
    : QObject{parent},
    m_audio("Audio")
{
    connect(this, &WebRTC::gatheringComplited, [this] (const QString &peerID) {

        m_localDescription = descriptionToJson(m_peerConnections[peerID]->localDescription().value());
        Q_EMIT localDescriptionGenerated(peerID, m_localDescription);

        if (m_isOfferer)
            Q_EMIT this->offerIsReady(peerID, m_localDescription);
        else
            Q_EMIT this->answerIsReady(peerID, m_localDescription);
    });
}

WebRTC::~WebRTC()
{}


/**
 * ====================================================
 * ================= public methods ===================
 * ====================================================
 */

void WebRTC::init(const QString &id, bool isOfferer)
{
    // Initialize WebRTC using libdatachannel library
    rtc::InitLogger(rtc::LogLevel::Info, NULL);
    // Create an instance of rtc::Configuration to Set up ICE configuration
    rtc::Configuration config;
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");
    // Add a STUN server to help peers find their public IP addresses
    m_config = config;
    // Add a TURN server for relaying media if a direct connection can't be established

    // Set up the audio stream configuration
}

void WebRTC::addPeer(const QString &peerId)
{
    // Create and add a new peer connection
    auto newPeer = std::make_shared<rtc::PeerConnection>(m_config);
    m_peerConnections.insert(peerId, newPeer);
    // Set up a callback for when the local description is generated

    newPeer->onLocalDescription([this, peerId](const rtc::Description &description) {
        // The local description should be emitted using the appropriate signals based on the peer's role (offerer or answerer)
        emit localDescriptionGenerated(peerId, QString::fromStdString(description.generateSdp()));
    });



    // Set up a callback for handling local ICE candidates
    newPeer->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        // Emit the local candidates using the localCandidateGenerated signal
        emit localCandidateGenerated(peerId,
                                     QString::fromStdString(candidate.candidate()),
                                     QString::fromStdString(candidate.mid()));
    });



    // Set up a callback for when the state of the peer connection changes
    newPeer->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        // Handle different states like New, Connecting, Connected, Disconnected, etc.
        switch (state) {
        case rtc::PeerConnection::State::New:
            break;
        case rtc::PeerConnection::State::Connecting:
            break;
        case rtc::PeerConnection::State::Connected:
            break;
        case rtc::PeerConnection::State::Disconnected:
            break;
        case rtc::PeerConnection::State::Closed:
            break;
        case rtc::PeerConnection::State::Failed:
            break;
        default:
            break;
        }
    });



    // Set up a callback for monitoring the gathering state
    newPeer->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        // When the gathering is complete, emit the gatheringComplited signal
        if (rtc::PeerConnection::GatheringState::Complete == state)
            emit gatheringComplited(peerId);
    });

    // Set up a callback for handling incoming tracks
    newPeer->onTrack([this, peerId] (std::shared_ptr<rtc::Track> track) {
        // handle the incoming media stream, emitting the incommingPacket signal if a stream is received
        
    });
    // Add an audio track to the peer connection
    rtc::Description::Audio audio = rtc::Description::Audio("audio",
                                                            rtc::Description::Direction::SendRecv);
    auto track = newPeer->addTrack(audio);
    m_peerTracks.insert(peerId, track);
}

// Set the local description for the peer's connection
void WebRTC::generateOfferSDP(const QString &peerId)
{
    std::shared_ptr<rtc::PeerConnection> connection = m_peerConnections[peerId];
    connection->setLocalDescription(rtc::Description::Type::Offer);
}

// Generate an answer SDP for the peer
void WebRTC::generateAnswerSDP(const QString &peerId)
{
}

// Add an audio track to the peer connection
void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName)
{
    // Add an audio track to the peer connection

    // Handle track events

    track->onMessage([this, peerId] (rtc::message_variant data) {

    });

    track->onFrame([this] (rtc::binary frame, rtc::FrameInfo info) {

    });

}

// Sends audio track data to the peer

void WebRTC::sendTrack(const QString &peerId, const QByteArray &buffer)
{

        // Create the RTP header and initialize an RtpHeader struct


    // Create the RTP packet by appending the RTP header and the payload buffer


    // Send the packet, catch and handle any errors that occur during sending

}


/**
 * ====================================================
 * ================= public slots =====================
 * ====================================================
 */

// Set the remote SDP description for the peer that contains metadata about the media being transmitted
void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{
    std::shared_ptr<rtc::PeerConnection> connection = m_peerConnections[peerID];
    std::string type = m_isOfferer ? "offer" : "answer";
    connection->setRemoteDescription(rtc::Description(sdp.toStdString(), type));
}

// Add remote ICE candidates to the peer connection
void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{
    std::shared_ptr<rtc::PeerConnection> connection = m_peerConnections[peerID];
    connection->addRemoteCandidate(rtc::Candidate(candidate.toStdString(), sdpMid.toStdString()));
}



/*
 * ====================================================
 * ================= private methods ==================
 * ====================================================
 */

// Utility function to read the rtc::message_variant into a QByteArray
QByteArray WebRTC::readVariant(const rtc::message_variant &data)
{
    if (std::holds_alternative<rtc::binary>(data)) {
        const rtc::binary &binData = std::get<rtc::binary>(data);
        return QByteArray(reinterpret_cast<const char *>(binData.data()),
                          static_cast<int>(binData.size()));

    } else if (std::holds_alternative<std::string>(data)) {
        const std::string &strData = std::get<std::string>(data);
        return QByteArray(strData.c_str(), static_cast<int>(strData.size()));
    }
    return QByteArray();
}

// Utility function to convert rtc::Description to JSON format
QString WebRTC::descriptionToJson(const rtc::Description &description)
{
    auto temp = QString("{\"type\": \"%1\", \"sdp\": \"%2\"}");
    auto typeString = QString::fromStdString(description.typeString());
    auto sdp = QString::fromStdString(description);
    QJsonDocument doc = QJsonDocument::fromJson(temp.arg(typeString, sdp).toUtf8());
    return doc.toJson();
}

// Retrieves the current bit rate
int WebRTC::bitRate() const
{

}

// Set a new bit rate and emit the bitRateChanged signal
void WebRTC::setBitRate(int newBitRate)
{

}

// Reset the bit rate to its default value
void WebRTC::resetBitRate()
{

}

// Sets a new payload type and emit the payloadTypeChanged signal
void WebRTC::setPayloadType(int newPayloadType)
{

}

// Resets the payload type to its default value
void WebRTC::resetPayloadType()
{

}

// Retrieve the current SSRC value
rtc::SSRC WebRTC::ssrc() const
{

}

// Set a new SSRC and emit the ssrcChanged signal
void WebRTC::setSsrc(rtc::SSRC newSsrc)
{

}

// Reset the SSRC to its default value
void WebRTC::resetSsrc()
{

}

// Retrieve the current payload type
int WebRTC::payloadType() const
{

}


/**
 * ====================================================
 * ================= getters setters ==================
 * ====================================================
 */

bool WebRTC::isOfferer() const
{

}

void WebRTC::setIsOfferer(bool newIsOfferer)
{

}

void WebRTC::resetIsOfferer()
{

}


