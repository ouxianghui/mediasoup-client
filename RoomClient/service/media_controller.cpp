#include "media_controller.h"
#include "Transport.hpp"
#include "api/peer_connection_interface.h"
#include "api/rtp_parameters.h"
#include "logger/u_logger.h"
#include "mediasoup_api.h"
#include "mac_capturer.h"

namespace vi {

MediaController::MediaController(std::shared_ptr<IMediasoupApi>& mediasoupApi,
                                 std::shared_ptr<mediasoupclient::SendTransport>& sendTransport,
                                 std::shared_ptr<mediasoupclient::RecvTransport>& recvTransport,
                                 rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>& pcf,
                                 std::shared_ptr<Options>& options)
    : _mediasoupApi(mediasoupApi)
    , _sendTransport(sendTransport)
    , _recvTransport(recvTransport)
    , _peerConnectionFactory(pcf)
    , _options(options)
{

}

MediaController::~MediaController()
{

}

void MediaController::init()
{
    configVideoEncodings();
}

void MediaController::destroy()
{
    if (_micProducer) {
        _micProducer->Close();
        _micProducer = nullptr;
    }

    if (_camProducer) {
        _camProducer->Close();
        _camProducer = nullptr;
    }

    if (!_dataConsumerMap.empty()) {
        for (auto& dataConsumer : _dataConsumerMap) {
            dataConsumer.second->Close();
        }
        _dataConsumerMap.clear();
    }

    if (!_consumerMap.empty()) {
        for (auto& consumer : _consumerMap) {
            consumer.second->Close();
        }
        _consumerMap.clear();
    }
}

void MediaController::addObserver(std::shared_ptr<IMediaControllerObserver> observer)
{
    UniversalObservable<IMediaControllerObserver>::addWeakObserver(observer, "mediasoup-client");
}

void MediaController::removeObserver(std::shared_ptr<IMediaControllerObserver> observer)
{
    UniversalObservable<IMediaControllerObserver>::removeObserver(observer);
}

void MediaController::configVideoEncodings()
{
    _encodings.clear();

    webrtc::RtpEncodingParameters ph;
    ph.rid = "h";
    ph.active = true;
    ph.max_bitrate_bps = 500000;
    ph.scale_resolution_down_by = 1;

    webrtc::RtpEncodingParameters pm;
    pm.rid = "m";
    pm.active = true;
    pm.max_bitrate_bps = 1000000;
    pm.scale_resolution_down_by = 2;

    webrtc::RtpEncodingParameters pl;
    pl.rid = "m";
    pl.active = true;
    pl.max_bitrate_bps = 500000;
    pl.scale_resolution_down_by = 4;

    _encodings.emplace_back(ph);
    _encodings.emplace_back(pm);
    _encodings.emplace_back(pl);
}

void MediaController::enableAudio(bool enabled)
{
    if (!_sendTransport) {
        DLOG("_sendTransport is null");
        return;
    }

    if (enabled) {
        rtc::scoped_refptr<webrtc::AudioTrackInterface> track = _peerConnectionFactory->CreateAudioTrack("mic-label", _peerConnectionFactory->CreateAudioSource(cricket::AudioOptions()));

        nlohmann::json codecOptions = nlohmann::json::object();
        codecOptions["opusStereo"] = true;
        codecOptions["opusDtx"] = true;

        mediasoupclient::Producer* producer = _sendTransport->Produce(this,
                                                                      track,
                                                                      nullptr,
                                                                      &codecOptions,
                                                                      nullptr);
        _micProducer.reset(producer);
    }
    else {
        if (!_mediasoupApi) {
            DLOG("_mediasoupApi is null");
            return;
        }

        if (!_micProducer) {
            DLOG("_micProducer is null");
            return;
        }

        _mediasoupApi->closeProducer(_micProducer->GetId(), [wself = weak_from_this()](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::BasicResponse> response){
            auto self = wself.lock();
            if (!self) {
                DLOG("RoomClient is null");
                return;
            }
            if (errorCode != 0) {
                DLOG("closeProducer failed, error code: {}, error info: {}", errorCode, errorInfo);
                return;
            }
            if (!response || !response->ok) {
                DLOG("response is null or response->ok == false");
                return;
            }
        });
    }
}

bool MediaController::isAudioEnabled()
{
    if (!_micProducer) {
        DLOG("_micProducer is null");
        return false;
    }
    return _micProducer->IsClosed();
}

void MediaController::muteAudio(bool muted)
{
    if (!_mediasoupApi) {
        DLOG("_mediasoupApi is null");
        return;
    }

    if (!_micProducer) {
        DLOG("_micProducer is null");
        return;
    }

    if (muted) {
        _micProducer->Pause();

        _mediasoupApi->pauseProducer(_micProducer->GetId(), [wself = weak_from_this()](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::BasicResponse> response){
            auto self = wself.lock();
            if (!self) {
                DLOG("RoomClient is null");
                return;
            }
            if (errorCode != 0) {
                DLOG("pauseProducer failed, error code: {}, error info: {}", errorCode, errorInfo);
                return;
            }
            if (!response || !response->ok) {
                DLOG("response is null or response->ok == false");
                return;
            }
        });
    }
    else {
        _micProducer->Resume();

        _mediasoupApi->resumeProducer(_micProducer->GetId(), [wself = weak_from_this()](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::BasicResponse> response){
            auto self = wself.lock();
            if (!self) {
                DLOG("RoomClient is null");
                return;
            }
            if (errorCode != 0) {
                DLOG("resumeProducer failed, error code: {}, error info: {}", errorCode, errorInfo);
                return;
            }
            if (!response || !response->ok) {
                DLOG("response is null or response->ok == false");
                return;
            }
        });
    }
}

bool MediaController::isAudioMuted()
{
    if (!_micProducer) {
        DLOG("_micProducer is null");
        return true;
    }
    return _micProducer->IsPaused();
}

void MediaController::enableVideo(bool enabled)
{
    if (!_sendTransport) {
        DLOG("_sendTransport is null");
        return;
    }

    if (enabled) {
        std::unique_ptr<MacCapturer> capturer = absl::WrapUnique(MacCapturer::Create(1280, 720, 30, 0));

        rtc::scoped_refptr<MacTrackSource> capturerSource = rtc::make_ref_counted<MacTrackSource>(std::move(capturer), false);

        DLOG("create capture source");
        if (capturerSource) {
            rtc::scoped_refptr<webrtc::VideoTrackInterface> track = _peerConnectionFactory->CreateVideoTrack("camera-track", capturerSource);

            nlohmann::json codecOptions = nlohmann::json::object();
            codecOptions["videoGoogleStartBitrate"] = 1000;

            mediasoupclient::Producer* producer = _sendTransport->Produce(this,
                                                                          track,
                                                                          _options->useSimulcast.value_or(false) ? &_encodings : nullptr,
                                                                          &codecOptions,
                                                                          nullptr);
            _camProducer.reset(producer);

            UniversalObservable<IMediaControllerObserver>::notifyObservers([wself = weak_from_this()](const auto& observer){
                auto self = wself.lock();
                if (!self) {
                    DLOG("RoomClient is null");
                    return;
                }
                observer->onCreateVideoTrack(self->_camProducer->GetId(), self->_camProducer->GetTrack());
            });
        }
    }
    else {
        if (!_mediasoupApi) {
            DLOG("_mediasoupApi is null");
            return;
        }

        if (!_camProducer) {
            DLOG("_camProducer is null");
            return;
        }

        _mediasoupApi->closeProducer(_camProducer->GetId(), [wself = weak_from_this()](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::BasicResponse> response){
            auto self = wself.lock();
            if (!self) {
                DLOG("RoomClient is null");
                return;
            }
            if (errorCode != 0) {
                DLOG("closeProducer failed, error code: {}, error info: {}", errorCode, errorInfo);
                return;
            }
            if (!response || !response->ok) {
                DLOG("response is null or response->ok == false");
                return;
            }
        });
    }
}

bool MediaController::isVideoEnabled()
{
    if (!_camProducer) {
        DLOG("_camProducer is null");
        return false;
    }
    return _camProducer->IsClosed();
}

void MediaController::muteAudio(const std::string& id, bool muted)
{
    if (_consumerMap.find(id) == _consumerMap.end()) {
        return;
    }

    if (!_mediasoupApi) {
        DLOG("_mediasoupApi is null");
        return;
    }

    auto consumer = _consumerMap[id];

    if (muted) {
        consumer->Pause();

        _mediasoupApi->pauseConsumer(consumer->GetId(), [wself = weak_from_this()](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::BasicResponse> response){
            auto self = wself.lock();
            if (!self) {
                DLOG("RoomClient is null");
                return;
            }
            if (errorCode != 0) {
                DLOG("pauseConsumer failed, error code: {}, error info: {}", errorCode, errorInfo);
                return;
            }
            if (!response || !response->ok) {
                DLOG("response is null or response->ok == false");
                return;
            }
        });
    }
    else {
        consumer->Resume();

        _mediasoupApi->resumeConsumer(consumer->GetId(), [wself = weak_from_this()](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::BasicResponse> response){
            auto self = wself.lock();
            if (!self) {
                DLOG("RoomClient is null");
                return;
            }
            if (errorCode != 0) {
                DLOG("resumeConsumer failed, error code: {}, error info: {}", errorCode, errorInfo);
                return;
            }
            if (!response || !response->ok) {
                DLOG("response is null or response->ok == false");
                return;
            }
        });
    }
}

bool MediaController::isAudioMuted(const std::string& id)
{
    if (_consumerMap.find(id) != _consumerMap.end()) {
        auto consumer = _consumerMap[id];
        return consumer->IsPaused();
    }
    return true;
}

void MediaController::createNewConsumer(std::shared_ptr<signaling::NewConsumerRequest> request)
{
    if (!request) {
        DLOG("request is null");
        return;
    }

    if (!_options->consume.value_or(false)) {
        DLOG("consuming is disabled");
        return;
    }

    if (!_recvTransport) {
        DLOG("_recvTransport is null");
        return;
    }

    nlohmann::json rtpParameters = nlohmann::json::parse(request->data->rtpParameters->toJsonStr());
    nlohmann::json appData = nlohmann::json::parse(request->data->appData->toJsonStr());
    mediasoupclient::Consumer* consumer = _recvTransport->Consume(this,
                                                                  request->data->id.value(),
                                                                  request->data->producerId.value(),
                                                                  request->data->kind.value(),
                                                                  &rtpParameters,
                                                                  appData);
    std::shared_ptr<mediasoupclient::Consumer> ptr;
    ptr.reset(consumer);
    _consumerMap[request->data->id.value()] = ptr;

    UniversalObservable<IMediaControllerObserver>::notifyObservers([wself = weak_from_this(), ptr](const auto& observer){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }
        if (ptr->GetKind() == "video") {
            observer->onCreateVideoTrack(ptr->GetId(), ptr->GetTrack());
            ptr->GetTrack()->set_enabled(true);
        }
    });

    if (!_mediasoupApi) {
        DLOG("_mediasoupApi is null");
        return;
    }

    auto response = std::make_shared<signaling::BasicResponse>();
    response->response = true;
    response->id = request->id;
    response->ok = true;
    _mediasoupApi->response(response);
}

void MediaController::createNewDataConsumer(std::shared_ptr<signaling::NewDataConsumerRequest> request)
{
    if (!request) {
        DLOG("request is null");
        return;
    }

    if (!_options->consume.value_or(false)) {
        DLOG("consuming is disabled");
        return;
    }

    if (!_options->datachannel.value_or(false)) {
        DLOG("datachannel is disabled");
        return;
    }

    if (!_recvTransport) {
        DLOG("_recvTransport is null");
        return;
    }

    nlohmann::json appData = nlohmann::json::parse(request->data->appData->toJsonStr());
    mediasoupclient::DataConsumer* consumer = _recvTransport->ConsumeData(this,
                                                                          request->data->id.value(),
                                                                          request->data->dataProducerId.value(),
                                                                          request->data->sctpStreamParameters->streamId.value(),
                                                                          request->data->label.value(),
                                                                          request->data->protocol.value(),
                                                                          appData);
    std::shared_ptr<mediasoupclient::DataConsumer> ptr;
    ptr.reset(consumer);
    _dataConsumerMap[request->data->id.value()] = ptr;

    if (!_mediasoupApi) {
        DLOG("_mediasoupApi is null");
        return;
    }

    auto response = std::make_shared<signaling::BasicResponse>();
    response->response = true;
    response->id = request->id;
    response->ok = true;
    _mediasoupApi->response(response);
}

void MediaController::OnTransportClose(mediasoupclient::Producer* producer)
{

}

void MediaController::OnTransportClose(mediasoupclient::Consumer* consumer)
{

}

void MediaController::OnOpen(mediasoupclient::DataProducer* dataProducer)
{

}

void MediaController::OnClose(mediasoupclient::DataProducer* dataProducer)
{

}

void MediaController::OnBufferedAmountChange(mediasoupclient::DataProducer* dataProducer, uint64_t sentDataSize)
{

}

void MediaController::OnTransportClose(mediasoupclient::DataProducer* dataProducer)
{

}

void MediaController::OnConnecting(mediasoupclient::DataConsumer* dataConsumer)
{

}

void MediaController::OnOpen(mediasoupclient::DataConsumer* dataConsumer)
{

}

void MediaController::OnClosing(mediasoupclient::DataConsumer* dataConsumer)
{

}

void MediaController::OnClose(mediasoupclient::DataConsumer* dataConsumer)
{

}

void MediaController::OnMessage(mediasoupclient::DataConsumer* dataConsumer, const webrtc::DataBuffer& buffer)
{

}

void MediaController::OnTransportClose(mediasoupclient::DataConsumer* dataConsumer)
{

}

void MediaController::onOpened()
{

}

void MediaController::onClosed()
{

}

// Request from SFU
void MediaController::onNewConsumer(std::shared_ptr<signaling::NewConsumerRequest> request)
{
    TMgr->thread("mediasoup-client")->PostTask([wself = weak_from_this(), request](){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }
        self->createNewConsumer(request);
    });
}

void MediaController::onNewDataConsumer(std::shared_ptr<signaling::NewDataConsumerRequest> request)
{
    TMgr->thread("mediasoup-client")->PostTask([wself = weak_from_this(), request](){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }
        self->createNewDataConsumer(request);
    });
}

// Notification from SFU
void MediaController::onProducerScore(std::shared_ptr<signaling::ProducerScoreNotification> notification)
{

}

void MediaController::onConsumerScore(std::shared_ptr<signaling::ConsumerScoreNotification> notification)
{

}

void MediaController::onNewPeer(std::shared_ptr<signaling::NewPeerNotification> notification)
{

}

void MediaController::onPeerClosed(std::shared_ptr<signaling::PeerClosedNotification> notification)
{

}

void MediaController::onPeerDisplayNameChanged(std::shared_ptr<signaling::PeerDisplayNameChangedNotification> notification)
{

}

void MediaController::onConsumerPaused(std::shared_ptr<signaling::ConsumerPausedNotification> notification)
{

}

void MediaController::onConsumerResumed(std::shared_ptr<signaling::ConsumerResumedNotification> notification)
{

}

void MediaController::onConsumerClosed(std::shared_ptr<signaling::ConsumerClosedNotification> notification)
{
    UniversalObservable<IMediaControllerObserver>::notifyObservers([wself = weak_from_this(), notification](const auto& observer){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }
        for (const auto& consumer : self->_consumerMap) {
            if (consumer.second->GetId() == notification->data->consumerId.value_or("") && consumer.second->GetKind() == "video") {
                observer->onRemoveVideoTrack(consumer.second->GetId(), consumer.second->GetTrack());
                self->_consumerMap.erase(consumer.second->GetId());
                return;
            }
        }
    });
}

void MediaController::onConsumerLayersChanged(std::shared_ptr<signaling::ConsumerLayersChangedNotification> notification)
{

}

void MediaController::onDataConsumerClosed(std::shared_ptr<signaling::DataConsumerClosedNotification> notification)
{

}

void MediaController::onDownlinkBwe(std::shared_ptr<signaling::DownlinkBweNotification> notification)
{

}

void MediaController::onActiveSpeaker(std::shared_ptr<signaling::ActiveSpeakerNotification> notification)
{

}

}
