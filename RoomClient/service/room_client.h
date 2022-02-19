#pragma once

#include <memory>
#include <unordered_map>
#include "i_room_client.h"
#include "i_signaling_observer.h"
#include "utils/universal_observable.hpp"
#include "Transport.hpp"
#include "Device.hpp"
#include "Producer.hpp"
#include "DataProducer.hpp"
#include "Consumer.hpp"
#include "DataConsumer.hpp"

namespace vi {

class IComponentFactory;
class ISignalingClient;
class IMediasoupApi;
class MacTrackSource;
class IParticipant;
class MediaController;
class ParticipantController;

class RoomClient :
        public IRoomClient,
        public ISignalingObserver,
        public mediasoupclient::SendTransport::Listener,
        public mediasoupclient::RecvTransport::Listener,
        public UniversalObservable<IRoomClientObserver>,
        public std::enable_shared_from_this<RoomClient> {
public:
    RoomClient(std::weak_ptr<IComponentFactory> wcf);

    ~RoomClient();

    void init() override;

    void destroy() override;

    void addObserver(std::shared_ptr<IRoomClientObserver> observer) override;

    void removeObserver(std::shared_ptr<IRoomClientObserver> observer) override;

    void join(const std::string& host, uint16_t port, const std::string& roomId, const std::string& displayName, std::shared_ptr<Options> options) override;

    void leave() override;

    std::shared_ptr<IMediaController> getMediaController() override;

    std::shared_ptr<IParticipantController> getParticipantController() override;

protected:
    // ISignalingObserver
    void onOpened() override;

    void onClosed() override;

    // Request from SFU
    void onNewConsumer(std::shared_ptr<signaling::NewConsumerRequest> request) override;

    void onNewDataConsumer(std::shared_ptr<signaling::NewDataConsumerRequest> request) override;

    // Notification from SFU
    void onProducerScore(std::shared_ptr<signaling::ProducerScoreNotification> notification) override;

    void onConsumerScore(std::shared_ptr<signaling::ConsumerScoreNotification> notification) override;

    void onNewPeer(std::shared_ptr<signaling::NewPeerNotification> notification) override;

    void onPeerClosed(std::shared_ptr<signaling::PeerClosedNotification> notification) override;

    void onPeerDisplayNameChanged(std::shared_ptr<signaling::PeerDisplayNameChangedNotification> notification) override;

    void onConsumerPaused(std::shared_ptr<signaling::ConsumerPausedNotification> notification) override;

    void onConsumerResumed(std::shared_ptr<signaling::ConsumerResumedNotification> notification) override;

    void onConsumerClosed(std::shared_ptr<signaling::ConsumerClosedNotification> notification) override;

    void onConsumerLayersChanged(std::shared_ptr<signaling::ConsumerLayersChangedNotification> notification) override;

    void onDataConsumerClosed(std::shared_ptr<signaling::DataConsumerClosedNotification> notification) override;

    void onDownlinkBwe(std::shared_ptr<signaling::DownlinkBweNotification> notification) override;

    void onActiveSpeaker(std::shared_ptr<signaling::ActiveSpeakerNotification> notification) override;

protected:
    // Transport::Listener
    std::future<void> OnConnect(mediasoupclient::Transport* transport, const nlohmann::json& dtlsParameters) override;

    void OnConnectionStateChange(mediasoupclient::Transport* transport, const std::string& connectionState) override;

    std::future<std::string> OnProduce(mediasoupclient::SendTransport* transport, const std::string& kind, nlohmann::json rtpParameters, const nlohmann::json& appData) override;

    std::future<std::string> OnProduceData(mediasoupclient::SendTransport* transport, const nlohmann::json& sctpStreamParameters, const std::string& label, const std::string& protocol, const nlohmann::json& appData) override;

private:
    void getRouterRtpCapabilities();

    void joinImpl();

    void onLoadMediasoupDevice(std::shared_ptr<signaling::GetRouterRtpCapabilitiesResponse> response);

    void createSendTransport();

    void createRecvTransport();

    void requestCreateTransport(bool forceTcp, bool producing, bool consuming);

    void onCreateSendTransport(std::shared_ptr<signaling::CreateWebRtcTransportResponse> transportInfo);

    void onCreateRecvTransport(std::shared_ptr<signaling::CreateWebRtcTransportResponse> transportInfo);

    void createTransportImpl(bool producing, bool consuming, std::shared_ptr<signaling::CreateWebRtcTransportResponse> transportInfo);

    void configWebrtc();

    void _onConnect(mediasoupclient::Transport* transport, const nlohmann::json& dtlsParameters);

    std::string _onProduce(mediasoupclient::SendTransport* transport, const std::string& kind, nlohmann::json rtpParameters, const nlohmann::json& appData);

    std::string _onProduceData(mediasoupclient::SendTransport* transport, const nlohmann::json& sctpStreamParameters, const std::string& label, const std::string& protocol, const nlohmann::json& appData);

private:
    std::weak_ptr<IComponentFactory> _wcf;

    std::string _hostname;
    uint16_t _port;
    std::string _peerId;
    std::string _roomId;
    std::string _displayName;

    std::shared_ptr<Options> _options;

    std::shared_ptr<ISignalingClient> _signalingClient;
    std::shared_ptr<IMediasoupApi> _mediasoupApi;

    std::shared_ptr<mediasoupclient::Device> _mediasoupDevice;

    std::shared_ptr<mediasoupclient::SendTransport> _sendTransport;
    std::shared_ptr<mediasoupclient::RecvTransport> _recvTransport;

    std::shared_ptr<mediasoupclient::PeerConnection::Options> _peerConnectionOptions;

    std::unique_ptr<rtc::Thread> _networkThread;
    std::unique_ptr<rtc::Thread> _signalingThread;
    std::unique_ptr<rtc::Thread> _workerThread;
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _peerConnectionFactory;

    std::weak_ptr<MediaController> _mediaController;
    std::weak_ptr<ParticipantController> _participantController;

    std::shared_ptr<IMediaController> _mediaControllerProxy;
    std::shared_ptr<IParticipantController> _participantControllerProxy;
};


}
