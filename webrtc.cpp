#include "webrtc.h"
#include <QtEndian>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

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
    connect(this, &WebRTC::gatheringCompleted, [this] (const QString &peerID) {
        if (!m_gatheringCompleted) return;
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
    rtc::InitLogger(rtc::LogLevel::Error, NULL);

    // Create an instance of rtc::Configuration to Set up ICE configuration
    rtc::Configuration config;

    // Add a STUN server to help peers find their public IP addresses
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");

    // Add a TURN server for relaying media if a direct connection can't be established
    // m_config.iceServers.emplace_back("turn:your_turn_server.com", "username", "password");

    m_config = config;

    // Set up the audio stream configuration
    m_audio.setBitrate(m_bitRate);
    m_audio.addSSRC(m_ssrc, "audio-send");
    m_audio.addOpusCodec(m_payloadType);

    m_isOfferer = isOfferer;
    m_localId = id;
}

void WebRTC::addPeer(const QString &peerId)
{
    if (m_peerTracks.contains(peerId)) {
        return;
    }
    // Create and add a new peer connection
    auto newPeer = std::make_shared<rtc::PeerConnection>(m_config);
    m_peerConnections.insert(peerId, newPeer);

    // Set up a callback for when the local description is generated
    newPeer->onLocalDescription([this, peerId](const rtc::Description &description) {
        if (m_gatheringCompleted) return;
        auto typeString = QString::fromStdString(description.typeString());
        auto sdp = QString::fromStdString(description);
        m_isOfferer = (typeString == "offer");
        m_localDescription = descriptionToJson(description);

    });


    // Set up a callback for handling local ICE candidates
    newPeer->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        // Emit the local candidates using the localCandidateGenerated signal
        if (!m_gatheringCompleted) return;
        Q_EMIT localCandidateGenerated(peerId,
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
            Q_EMIT rtcConnected();
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
        if (rtc::PeerConnection::GatheringState::Complete == state) {
            m_gatheringCompleted = true;
            Q_EMIT gatheringCompleted(peerId);
        }
    });

    // Set up a callback for handling incoming tracks
    newPeer->onTrack([this, peerId](std::shared_ptr<rtc::Track> track) {
        // handle the incoming media stream, emitting the incommingPacket signal if a stream is received
        m_peerTracks[peerId] = track;
        track->onMessage([this, peerId](rtc::message_variant data) {
            qDebug() << "on message called in add peer";
        });
    });


    // Add an audio track to the peer connection
    // rtc::Description::Audio audio = rtc::Description::Audio("audio",
    //                                                         rtc::Description::Direction::SendRecv);
    // auto track = newPeer->addTrack(audio);
    // m_peerTracks.insert(peerId, track);
    addAudioTrack(peerId, "audio");
}

// Set the local description for the peer's connection
void WebRTC::generateOfferSDP(const QString &peerId)
{
    setIsOfferer(true);
    std::shared_ptr<rtc::PeerConnection> connection = m_peerConnections[peerId];
    connection->setLocalDescription(rtc::Description::Type::Offer);
}

// Generate an answer SDP for the peer
void WebRTC::generateAnswerSDP(const QString &peerId)
{
    if (!m_peerConnections.contains(peerId))
        addPeer(peerId);
    setIsOfferer(false);

    std::shared_ptr<rtc::PeerConnection> connection = m_peerConnections[peerId];
    connection->localDescription()->generateSdp();
}

// Add an audio track to the peer connection
void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName)
{
    // Add an audio track to the peer connection
    auto track = m_peerConnections[peerId]->addTrack(m_audio);

    // Handle track events
    track->onMessage([this, peerId](rtc::message_variant data) {
        QByteArray receivedData = readVariant(data);
        Q_EMIT incommingPacket(peerId, receivedData, receivedData.size());
    });

    m_peerTracks[peerId] = track;
}

// Sends audio track data to the peer
void WebRTC::sendTrack(const QString &peerId, const QByteArray &buffer)
{
    // Create the RTP header and initialize an RtpHeader struct
    RtpHeader header;
    header.first = 0x80; // RTP version 2
    header.marker = 0;
    header.payloadType = m_payloadType;
    header.sequenceNumber = qToBigEndian(m_sequenceNumber++);
    header.timestamp = qToBigEndian(getCurrentTimestamp());
    header.ssrc = qToBigEndian(m_ssrc);


    // Create the RTP packet by appending the RTP header and the payload buffer
    QByteArray packet;
    packet.append(reinterpret_cast<const char*>(&header), sizeof(RtpHeader));
    packet.append(buffer);


    // Send the packet, catch and handle any errors that occur during sending
    try {
        if (m_peerTracks.contains(peerId)) {
            m_peerTracks[peerId]->send(packet.toStdString());
        }
    } catch (const std::exception& e) {
        qWarning() << "Failed to send track data:" << e.what();
    }

}


/**
 * ====================================================
 * ================= public slots =====================
 * ====================================================
 */

// Set the remote SDP description for the peer that contains metadata about the media being transmitted
void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{
    // Set the remote SDP description for the peer that contains metadata about the media being transmitted
    if (m_peerConnections.contains(peerID))
    {
        std::shared_ptr<rtc::PeerConnection> connection = m_peerConnections[peerID];
        QJsonDocument doc = QJsonDocument::fromJson(sdp.toUtf8());
        QJsonObject jsonObj = doc.object();
        QString type = jsonObj.value("type").toString();
        QString sdpValue = jsonObj.value("sdp").toString();
        m_isOfferer = (type != "offer");
        connection->setRemoteDescription(rtc::Description(sdpValue.toStdString(), type.toStdString()));
    }
}

// Add remote ICE candidates to the peer connection
void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{
    try{
        if (m_peerConnections.contains(peerID)) {
            m_peerConnections[peerID]->addRemoteCandidate(rtc::Candidate(candidate.toStdString(), sdpMid.toStdString()));
        }
    }
    catch (const std::exception& e) {
        qWarning() << "Failed to set remote candidate" << e.what();
    }
}



/*
 * ====================================================
 * ================= private methods ==================
 * ====================================================
 */

// Utility function to read the rtc::message_variant into a QByteArray
QByteArray WebRTC::readVariant(const rtc::message_variant &data)
{
    QByteArray resultData;
    if (std::holds_alternative<rtc::binary>(data)) {
        const rtc::binary &binData = std::get<rtc::binary>(data);
        resultData =  QByteArray(reinterpret_cast<const char *>(binData.data()),
                          static_cast<int>(binData.size()));

    } else if (std::holds_alternative<std::string>(data)) {
        const std::string &strData = std::get<std::string>(data);
        resultData =  QByteArray(strData.c_str(), static_cast<int>(strData.size()));
    }
    if (resultData.size())
        resultData.remove(0, sizeof(RtpHeader));

    return resultData;
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
    return m_bitRate;
}

// Set a new bit rate and emit the bitRateChanged signal
void WebRTC::setBitRate(int newBitRate)
{
    m_bitRate = newBitRate;
}

// Reset the bit rate to its default value
void WebRTC::resetBitRate()
{
    m_bitRate = 48000;
}

// Sets a new payload type and emit the payloadTypeChanged signal
void WebRTC::setPayloadType(int newPayloadType)
{
    m_payloadType = newPayloadType;
}

// Resets the payload type to its default value
void WebRTC::resetPayloadType()
{
    m_payloadType = 111;
}

// Retrieve the current SSRC value
rtc::SSRC WebRTC::ssrc() const
{
    return m_ssrc;
}

// Set a new SSRC and emit the ssrcChanged signal
void WebRTC::setSsrc(rtc::SSRC newSsrc)
{
    m_ssrc = newSsrc;
}

// Reset the SSRC to its default value
void WebRTC::resetSsrc()
{
    m_ssrc = 2;
}

// Retrieve the current payload type
int WebRTC::payloadType() const
{
    return m_payloadType;
}


/**
 * ====================================================
 * ================= getters setters ==================
 * ====================================================
 */

bool WebRTC::isOfferer() const
{
    return m_isOfferer;
}

void WebRTC::setIsOfferer(bool newIsOfferer)
{
    m_isOfferer = newIsOfferer;
}

void WebRTC::resetIsOfferer()
{
    m_isOfferer = false;
}
