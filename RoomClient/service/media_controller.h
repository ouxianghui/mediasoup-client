#pragma once

#include <memory>
#include "i_media_controller.h"
#include "i_media_controller_observer.h"
#include "i_signaling_observer.h"
#include "utils/universal_observable.hpp"
#include "Producer.hpp"
#include "DataProducer.hpp"
#include "Consumer.hpp"
#include "DataConsumer.hpp"
#include "options.h"
#include "signaling_models.h"

namespace mediasoupclient {
    class SendTransport;
    class RecvTransport;
}

namespace vi {

class IMediasoupApi;
class MacTrackSource;

class MediaController :
        public IMediaController,
        public ISignalingObserver,
        public mediasoupclient::Producer::Listener,
        public mediasoupclient::Consumer::Listener,
        public mediasoupclient::DataProducer::Listener,
        public mediasoupclient::DataConsumer::Listener,
        public UniversalObservable<IMediaControllerObserver>,
        public std::enable_shared_from_this<MediaController>
{
public:
    MediaController(std::shared_ptr<IMediasoupApi>& mediasoupApi,
                    std::shared_ptr<mediasoupclient::SendTransport>& sendTransport,
                    std::shared_ptr<mediasoupclient::RecvTransport>& recvTransport,
                    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>& pcf,
                    std::shared_ptr<Options>& options);

    ~MediaController();

    void init() override;

    void destroy() override;

    void addObserver(std::shared_ptr<IMediaControllerObserver> observer) override;

    void removeObserver(std::shared_ptr<IMediaControllerObserver> observer) override;

    void enableAudio(bool enabled) override;

    bool isAudioEnabled() override;

    void muteAudio(bool muted) override;

    bool isAudioMuted() override;

    void enableVideo(bool enabled) override;

    bool isVideoEnabled() override;

    void muteAudio(const std::string& id, bool muted) override;

    bool isAudioMuted(const std::string& id) override;

protected:
    // Producer::Listener
    void OnTransportClose(mediasoupclient::Producer* producer) override;

    // Consumer::Listener
    void OnTransportClose(mediasoupclient::Consumer* consumer) override;

    // DataProducer::Listener
    void OnOpen(mediasoupclient::DataProducer* dataProducer) override;

    void OnClose(mediasoupclient::DataProducer* dataProducer) override;

    void OnBufferedAmountChange(mediasoupclient::DataProducer* dataProducer, uint64_t sentDataSize) override;

    void OnTransportClose(mediasoupclient::DataProducer* dataProducer) override;

    // DataConsumer::Listener
    void OnConnecting(mediasoupclient::DataConsumer* dataConsumer) override;

    void OnOpen(mediasoupclient::DataConsumer* dataConsumer) override;

    void OnClosing(mediasoupclient::DataConsumer* dataConsumer) override;

    void OnClose(mediasoupclient::DataConsumer* dataConsumer) override;

    void OnMessage(mediasoupclient::DataConsumer* dataConsumer, const webrtc::DataBuffer& buffer) override;

    void OnTransportClose(mediasoupclient::DataConsumer* dataConsumer) override;

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

private:
     void configVideoEncodings();

     void createNewConsumer(std::shared_ptr<signaling::NewConsumerRequest> request);

     void createNewDataConsumer(std::shared_ptr<signaling::NewDataConsumerRequest> request);

     void onCamProducerClosed();

private:
     std::shared_ptr<IMediasoupApi>& _mediasoupApi;

     std::shared_ptr<mediasoupclient::SendTransport>& _sendTransport;
     std::shared_ptr<mediasoupclient::RecvTransport>& _recvTransport;

     rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>& _peerConnectionFactory;

     std::shared_ptr<Options>& _options;

     std::vector<webrtc::RtpEncodingParameters> _encodings;

     std::shared_ptr<mediasoupclient::Producer> _micProducer;
     std::shared_ptr<mediasoupclient::Producer> _camProducer;
     rtc::scoped_refptr<MacTrackSource> _capturerSource;

     // key: consumer id
     std::unordered_map<std::string, std::shared_ptr<mediasoupclient::Consumer>> _consumerMap;

     // key: data consumer id
     std::unordered_map<std::string, std::shared_ptr<mediasoupclient::DataConsumer>> _dataConsumerMap;
};

}
