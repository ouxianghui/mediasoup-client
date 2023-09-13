#include "room_client.h"
#include "component_factory.h"
#include "signaling_client.h"
#include "mediasoup_api.h"
#include "signaling_client.h"
#include "logger/spd_logger.h"
#include "utils/string_utils.h"
#include "signaling_models.h"
#include "json/serialization.hpp"
#include "PeerConnection.hpp"
#include "api/peer_connection_interface.h"
#include "api/media_stream_interface.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/create_peerconnection_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/task_queue/default_task_queue_factory.h"
#include "modules/audio_device/include/audio_device.h"
#include "Producer.hpp"
#include "DataProducer.hpp"
#include "Consumer.hpp"
#include "DataConsumer.hpp"
#include "media_controller.h"
#include "participant_controller.h"
#include "engine.h"
#include "rtc_context.hpp"
#include "rtc_base/thread.h"

namespace {

std::string getProtooUrl(const std::string& hostname, uint16_t port, const std::string& roomId, const std::string& peerId)
{
    std::stringstream sstr;
    sstr << "wss://" << hostname << ":" << port << "/?roomId=" << roomId << "&peerId=" << peerId;
    return sstr.str();
}

}

namespace vi {

RoomClient::RoomClient(std::weak_ptr<IComponentFactory> wcf, std::shared_ptr<RTCContext> rtcContext, rtc::Thread* mediasoupThread, rtc::Thread* signalingThread)
    : _wcf(wcf)
    , _rtcContext(rtcContext)
    , _mediasoupThread(mediasoupThread)
    , _signalingThread(signalingThread)
{

}

RoomClient::~RoomClient()
{
    DLOG("~RoomClient()");
}

void RoomClient::init()
{
    auto cf = _wcf.lock();
    //const auto& threadProvider = cf->getThreadProvider();
    //threadProvider->thread("main")->PostTask(RTC_FROM_HERE, [this]() {
        configure();
     //});

    if (!_signalingClient) {
        _signalingClient = std::make_shared<SignalingClient>(_signalingThread);
        _signalingClient->init();
        _signalingClient->addObserver(shared_from_this());
    }

    if (_signalingClient && !_mediasoupApi) {
        _mediasoupApi = std::make_shared<MediasoupApi>(_signalingClient);
        _mediasoupApi->init();
    }
}

void RoomClient::destroy()
{
    if (_mediasoupApi) {
        _mediasoupApi->destroy();
        _mediasoupApi = nullptr;
    }

    if (_signalingClient) {
        _signalingClient->removeObserver(shared_from_this());
        _signalingClient->destroy();
        _signalingClient = nullptr;
    }

    destroyComponents();
}

void RoomClient::addObserver(std::shared_ptr<IRoomClientObserver> observer, rtc::Thread* callbackThread)
{
    UniversalObservable<IRoomClientObserver>::addWeakObserver(observer, callbackThread);
}

void RoomClient::removeObserver(std::shared_ptr<IRoomClientObserver> observer)
{
    UniversalObservable<IRoomClientObserver>::removeObserver(observer);
}

void RoomClient::configure()
{
    rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;

    if (_rtcContext) {
        peerConnectionFactory = _rtcContext->factory();
    }

    _peerConnectionOptions.reset(new mediasoupclient::PeerConnection::Options());
    _peerConnectionOptions->config.set_dscp(true);
    _peerConnectionOptions->factory = peerConnectionFactory != nullptr ? peerConnectionFactory : nullptr;
}


void RoomClient::join(const std::string& hostname, uint16_t port, const std::string& roomId, const std::string& displayName, std::shared_ptr<Options> options)
{
    if (_state != RoomState::CLOSED) {
        DLOG("There is already an ongoing meeting");
        return;
    }

    if (hostname.empty()) {
        DLOG("host name is null");
        return;
    }
    _hostname = hostname;
    _port = port;

    if (roomId.empty()) {
        DLOG("room id is null");
        return;
    }
    _roomId = roomId;

    if (displayName.empty()) {
        DLOG("display name id is null");
        return;
    }
    _displayName = displayName;

    if (options) {
        _options = options;
    }
    else {
        _options.reset(new Options());
    }

    _peerId = StringUtils::randomString(8);

    if (_signalingClient) {
        _signalingClient->disconnect();
        std::string url = getProtooUrl(_hostname, port, _roomId, _peerId);
        DLOG("protoo url: {}", url);
        _signalingClient->connect(url, "protoo");

        _state = RoomState::CONNECTING;
        onRoomStateChanged(_state);
    }
    else {
        DLOG("_signalingClient is null");
    }
}

void RoomClient::leave()
{
    if (_state == RoomState::CLOSED) {
        DLOG("already closed");
        return;
    }
    if (_signalingClient) {
        _signalingClient->disconnect();
        _state = RoomState::CLOSED;
        onRoomStateChanged(_state);
    }
    else {
        DLOG("_signalingClient is null");
    }
}

RoomState RoomClient::getRoomState()
{
    return _state;
}

void RoomClient::enableAudio(bool enabled)
{
    _mediaController->enableAudio(enabled);
}

bool RoomClient::isAudioEnabled()
{
    return _mediaController->isAudioEnabled();
}

void RoomClient::muteAudio(bool muted)
{
    _mediaController->muteAudio(muted);
}

bool RoomClient::isAudioMuted()
{
    return _mediaController->isAudioMuted();
}

void RoomClient::enableVideo(bool enabled)
{
    _mediaController->enableVideo(enabled);
}

bool RoomClient::isVideoEnabled()
{
    return _mediaController->isVideoEnabled();
}

int32_t RoomClient::speakingVolume()
{
    return _volume;
}

std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>> RoomClient::getVideoTracks()
{
    return _mediaController->getLocalVideoTracks();
}

std::shared_ptr<IParticipantController> RoomClient::getParticipantController()
{
    return _participantController ? _participantController->proxy() : nullptr;
}

void RoomClient::getRouterRtpCapabilities()
{
    if (!_mediasoupApi) {
        DLOG("_mediasoupApi is null");
        return;
    }

    _mediasoupApi->getRouterRtpCapabilities([wself = weak_from_this()](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::GetRouterRtpCapabilitiesResponse> response){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }
        if (errorCode != 0) {
            DLOG("getRouterRtpCapabilities failed, error code: {}, error info: {}", errorCode, errorInfo);
            return;
        }
        if (!response || !response->ok) {
            DLOG("response is null or response->ok == false");
            return;
        }
        self->_mediasoupThread->PostTask(RTC_FROM_HERE, [wself, response](){
            auto self = wself.lock();
            if (!self) {
                DLOG("RoomClient is null");
                return;
            }
            self->onLoadMediasoupDevice(response);
        });

    });
}

void RoomClient::onLoadMediasoupDevice(std::shared_ptr<signaling::GetRouterRtpCapabilitiesResponse> response)
{
    nlohmann::json rtpCapabilities = nlohmann::json::parse(response->data->toJsonStr());
    DLOG("rtpCapabilities: {}", response->data->toJsonStr());
    if (!_mediasoupDevice) {
        _mediasoupDevice = std::make_shared<mediasoupclient::Device>();
    }
    _mediasoupDevice->Load(rtpCapabilities, _peerConnectionOptions.get());
    if (_mediasoupDevice->IsLoaded()) {
        if (_options->produce.value_or(false)) {
            createSendTransport();
        }
        else if (_options->consume.value_or(false)) {
            createRecvTransport();
        }
        else {
            joinImpl();
        }
    }
}

void RoomClient::createSendTransport()
{
    requestCreateTransport(false, true, false);
}

void RoomClient::createRecvTransport()
{
    requestCreateTransport(false, false, true);
}

void RoomClient::requestCreateTransport(bool forceTcp, bool producing, bool consuming)
{
    if (!_mediasoupApi) {
        DLOG("_mediasoupApi is null");
        return;
    }

    auto request = std::make_shared<signaling::CreateWebRtcTransportRequest>();
    request->data = signaling::CreateWebRtcTransportRequest::Data();
    if (_options->datachannel.value_or(false)) {
        auto caps = _mediasoupDevice->GetSctpCapabilities();
        std::string json(caps.dump().c_str());
        DLOG("rtpCapabilities: {}", json);
        if (json.empty()) {
            return;
        }
        std::string err;
        auto sctpCapabilities = fromJsonString<signaling::CreateWebRtcTransportRequest::SCTPCapabilities>(json, err);
        if (!err.empty()) {
            DLOG("parse response failed: {}", err);
            return;
        }
        request->data->sctpCapabilities = *sctpCapabilities;
    }
    request->data->consuming = consuming;
    request->data->forceTcp = forceTcp;
    request->data->producing = producing;
    DLOG("requestCreateTransport, producing: {}, consuming: {}", producing, consuming);
    _mediasoupApi->createWebRtcTransport(request, [wself = weak_from_this(), consuming, producing](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::CreateWebRtcTransportResponse> response){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }
        if (errorCode != 0) {
            DLOG("createWebRtcTransport failed, error code: {}, error info: {}", errorCode, errorInfo);
            return;
        }

        if (!response || !response->ok) {
            DLOG("response is null or response->ok == false");
            return;
        }
        DLOG("createWebRtcTransport, producing: {}, consuming: {}", producing, consuming);

        self->_mediasoupThread->PostTask(RTC_FROM_HERE, [wself, producing, consuming, response](){
            auto self = wself.lock();
            if (!self) {
                DLOG("RoomClient is null");
                return;
            }
            if (producing) {
                self->onCreateSendTransport(response);
            }
            else if (consuming) {
                self->onCreateRecvTransport(response);
            }
        });
    });
}

void RoomClient::onCreateSendTransport(std::shared_ptr<signaling::CreateWebRtcTransportResponse> transportInfo)
{
    createTransportImpl(true, false, transportInfo);
    if (_options->consume.value_or(false)) {
        createRecvTransport();
    }
    else {
        joinImpl();
    }
}

void RoomClient::onCreateRecvTransport(std::shared_ptr<signaling::CreateWebRtcTransportResponse> transportInfo)
{
    createTransportImpl(false, true, transportInfo);
    joinImpl();
}

void RoomClient::createTransportImpl(bool producing, bool consuming, std::shared_ptr<signaling::CreateWebRtcTransportResponse> transportInfo)
{
    if (!_mediasoupDevice && _peerConnectionOptions) {
        DLOG("_mediasoupDevice is null");
        return;
    }
    DLOG("createTransportImpl, producing: {}, consuming: {}", producing, consuming);
    nlohmann::json iceParameters = nlohmann::json::parse(transportInfo->data->iceParameters->toJsonStr());

    nlohmann::json iceCandidates = nlohmann::json::array();
    for (auto& candidate : transportInfo->data->iceCandidates.value()) {
        DLOG("candidate: {}", candidate.toJsonStr());
        iceCandidates.emplace_back(nlohmann::json::parse(candidate.toJsonStr()));
    }
    transportInfo->data->dtlsParameters->role = "auto";
    nlohmann::json dtlsParameters = nlohmann::json::parse(transportInfo->data->dtlsParameters->toJsonStr());
    nlohmann::json sctpParameters = nlohmann::json::parse(transportInfo->data->sctpParameters->toJsonStr());

    if (producing) {
        auto sendTransport = _mediasoupDevice->CreateSendTransport(this, transportInfo->data->id.value_or(""), iceParameters, iceCandidates, dtlsParameters, sctpParameters, _peerConnectionOptions.get());
        _sendTransport.reset(sendTransport);
    }
    else if (consuming) {
        auto recvTransport = _mediasoupDevice->CreateRecvTransport(this, transportInfo->data->id.value_or(""), iceParameters, iceCandidates, dtlsParameters, sctpParameters, _peerConnectionOptions.get());
        _recvTransport.reset(recvTransport);
    }
}

void RoomClient::joinImpl()
{
    if (!_mediasoupApi) {
        DLOG("_mediasoupApi is null");
        return;
    }

    auto request = std::make_shared<signaling::JoinRequest>();
    request->data = signaling::JoinRequest::Data();
    request->data->displayName = _displayName;
    signaling::JoinRequest::Device device;
    device.flag = "macOS";
    device.name = "macOS";
    device.version = "1.0.0";
    request->data->device = device;
    if (_options->consume.value_or(false)) {
        auto caps = _mediasoupDevice->GetRtpCapabilities();
        std::string json(caps.dump().c_str());
        DLOG("rtpCapabilities: {}", json);
        if (json.empty()) {
            return;
        }
        std::string err;
        auto rtpCapabilities = fromJsonString<signaling::JoinRequest::RTPCapabilities>(json, err);
        if (!err.empty()) {
            DLOG("parse response failed: {}", err);
            return;
        }
        request->data->rtpCapabilities = *rtpCapabilities;
    }

    if (_options->datachannel.value_or(false)) {
        auto caps = _mediasoupDevice->GetSctpCapabilities();
        std::string json(caps.dump().c_str());
        DLOG("rtpCapabilities: {}", json);
        if (json.empty()) {
            return;
        }
        std::string err;
        auto sctpCapabilities = fromJsonString<signaling::JoinRequest::SCTPCapabilities>(json, err);
        if (!err.empty()) {
            DLOG("parse response failed: {}", err);
            return;
        }
        request->data->sctpCapabilities = *sctpCapabilities;
    }

    _mediasoupApi->join(request, [wself = weak_from_this()](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::JoinResponse> response){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }

        if (errorCode != 0) {
            DLOG("join failed, error code: {}, error info: {}", errorCode, errorInfo);
            return;
        }
        if (!response || !response->ok) {
            DLOG("response is null or response->ok == false");
            return;
        }
        
        self->_state = RoomState::CONNECTED;

        self->_mediasoupThread->PostTask(RTC_FROM_HERE, [wself, state = self->_state, response]() {
            auto self = wself.lock();
            if (!self) {
                DLOG("RoomClient is null");
                return;
            }

            self->onRoomStateChanged(state);

            if (!self->_participantController) {
                return;
            }

            if (auto pc = self->_participantController->impl()) {
                for (const auto& peer : response->data->peers.value_or(std::vector<signaling::JoinResponse::Peer>())) {
                    pc->createParticipant(peer.id.value_or(""), peer.displayName.value_or(""));
                }
            }
        });
    });
}

void RoomClient::onRoomStateChanged(vi::RoomState state)
{
    if (state == RoomState::CONNECTED) {
        //destroyComponents();
        createComponents();
    }
    else if (state == RoomState::CLOSED) {
        destroyComponents();
    }
    UniversalObservable<IRoomClientObserver>::notifyObservers([state](const auto& observer) {
        observer->onRoomStateChanged(state);
    });
}

void RoomClient::createComponents()
{
    if (!_mediaController) {
        auto factory = _rtcContext->factory();
        if (!factory) {
            ELOG("PeerConnectionFactory is null");
            return;
        }

        auto signalingThread = _rtcContext->signalingThread();
        if (!signalingThread) {
            ELOG("Signaling Thread is null");
            return;
        }

        auto adm = _rtcContext->adm();
        if (!adm) {
            ELOG("ADM is null");
            return;
        }

        auto mediaController = std::make_shared<MediaController>(_mediasoupDevice, _mediasoupApi, _sendTransport, _recvTransport, factory, _options, _mediasoupThread, signalingThread, adm);
        mediaController->init();
        _signalingClient->addObserver(mediaController);
        _mediaController = mediaController;
        _mediaController->addObserver(shared_from_this(), _mediasoupThread);
        _mediaController->enableVideo(true);
    }

    if (!_participantController) {
        auto participantControllerImpl = std::make_shared<ParticipantController>(_mediasoupThread, _mediaController);
        participantControllerImpl->init();
        auto participantControllerProxy = ParticipantControllerProxy::create(participantControllerImpl, _mediasoupThread);
        _participantController = std::make_shared<ProxyImpl<IParticipantController, ParticipantController>>(participantControllerProxy, participantControllerImpl);
        _signalingClient->addObserver(participantControllerImpl);
        _mediaController->addObserver(participantControllerImpl, _mediasoupThread);
    }
}

void RoomClient::destroyComponents()
{
    if (_participantController) {
        if (auto pc = _participantController->impl()) {
            pc->destroy();
        }
        _participantController = nullptr;
    }

    if (_mediasoupDevice) {
        _mediasoupDevice = nullptr;
    }

    if (_recvTransport) {
        _recvTransport->Close();
        _recvTransport = nullptr;
    }

    if (_sendTransport) {
        _sendTransport->Close();
        _sendTransport = nullptr;
    }

    if (_mediaController) {
        _mediaController->destroy();
        _mediaController = nullptr;
    }
}

std::future<void> RoomClient::OnConnect(mediasoupclient::Transport* transport, const nlohmann::json& dtlsParameters)
{
    std::future<void> fut = std::async(std::launch::async, [wself = weak_from_this(), transport, dtlsParameters](){
        auto self = wself.lock();
        if (!self) {
            return;
        }
        self->_onConnect(transport, dtlsParameters);
    });

    return fut;
}

void RoomClient::OnConnectionStateChange(mediasoupclient::Transport* transport, const std::string& connectionState)
{

}

std::future<std::string> RoomClient::OnProduce(mediasoupclient::SendTransport* transport,
                                               const std::string& kind,
                                               nlohmann::json rtpParameters,
                                               const nlohmann::json& appData)
{
    std::future<std::string> fut = std::async(std::launch::async, [wself = weak_from_this(), transport, kind, rtpParameters, appData](){
        auto self = wself.lock();
        if (!self) {
            return std::string();
        }
        return self->_onProduce(transport, kind, rtpParameters, appData);
    });

    return fut;
}

std::future<std::string> RoomClient::OnProduceData(mediasoupclient::SendTransport* transport,
                                                   const nlohmann::json& sctpStreamParameters,
                                                   const std::string& label,
                                                   const std::string& protocol,
                                                   const nlohmann::json& appData)
{
    std::future<std::string> fut = std::async(std::launch::async, [wself = weak_from_this(), transport, sctpStreamParameters, label, protocol, appData](){
        auto self = wself.lock();
        if (!self) {
            return std::string();
        }
        return self->_onProduceData(transport, sctpStreamParameters, label, protocol, appData);
    });

    return fut;
}

void RoomClient::_onConnect(mediasoupclient::Transport* transport, const nlohmann::json& dtlsParameters)
{
    std::promise<void> promise;
    std::future<void> future(promise.get_future());
    //DLOG("--> [transport], sendTransport: {}, _recvTransport: {}, transport: {}", _sendTransport->GetId(), _recvTransport->GetId(), transport->GetId());
    if (!_mediasoupApi) {
        DLOG("_mediasoupApi is null");
        return;
    }

    auto request = std::make_shared<signaling::ConnectWebRtcTransportRequest>();
    request->data = signaling::ConnectWebRtcTransportRequest::Data();
    request->data->transportId = transport->GetId();
    std::string json(dtlsParameters.dump().c_str());
    DLOG("rtpCapabilities: {}", json);
    if (json.empty()) {
        return;
    }
    std::string err;
    auto dtlsp = fromJsonString<signaling::ConnectWebRtcTransportRequest::DTLSParameters>(json, err);
    if (!err.empty()) {
        DLOG("parse response failed: {}", err);
        return;
    }

    request->data->dtlsParameters = *dtlsp;

    _mediasoupApi->connectWebRtcTransport(request, [wself = weak_from_this(), &promise, &transport](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::BasicResponse> response){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            promise.set_value();
            return;
        }
        if (errorCode != 0) {
            DLOG("connectWebRtcTransport failed, error code: {}, error info: {}", errorCode, errorInfo);
            promise.set_value();
            return;
        }
        if (!response || !response->ok) {
            DLOG("response is null or response->ok == false");
            promise.set_value();
            return;
        }
        promise.set_value();
    });

    return future.get();
}

std::string RoomClient::_onProduce(mediasoupclient::SendTransport* transport,
                                      const std::string& kind,
                                      nlohmann::json rtpParameters,
                                      const nlohmann::json& appData)
{
    std::promise<std::string> promise;
    std::future<std::string> future(promise.get_future());

    if (!_mediasoupApi) {
        DLOG("_mediasoupApi is null");
        return "";
    }

    auto request = std::make_shared<signaling::ProduceRequest>();
    request->data = signaling::ProduceRequest::Data();
    request->data->transportId = transport->GetId();
    request->data->kind = kind;
    std::string json(rtpParameters.dump().c_str());
    DLOG("rtpCapabilities: {}", json);
    if (json.empty()) {
        return "";
    }
    std::string err;
    auto rtpp = fromJsonString<signaling::ProduceRequest::RTPParameters>(json, err);
    if (!err.empty()) {
        DLOG("parse response failed: {}", err);
        return "";
    }
    request->data->rtpParameters = *rtpp;
    _mediasoupApi->produce(request, [wself = weak_from_this(), &promise](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::ProduceResponse> response){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            promise.set_value("");
            return;
        }
        if (errorCode != 0) {
            DLOG("produce failed, error code: {}, error info: {}", errorCode, errorInfo);
            promise.set_value("");
            return;
        }
        if (!response || !response->ok) {
            DLOG("response is null or response->ok == false");
            promise.set_value("");
            return;
        }
        promise.set_value(response->data->id.value_or(""));
    });

    return future.get();
}

std::string RoomClient::_onProduceData(mediasoupclient::SendTransport* transport,
                                          const nlohmann::json& sctpStreamParameters,
                                          const std::string& label,
                                          const std::string& protocol,
                                          const nlohmann::json& /*appData*/)
{
    std::promise<std::string> promise;
    std::future<std::string> future(promise.get_future());

    if (!_mediasoupApi) {
        DLOG("_mediasoupApi is null");
        return "";
    }

    auto request = std::make_shared<signaling::ProduceDataRequest>();
    request->data = signaling::ProduceDataRequest::Data();
    request->data->transportId = transport->GetId();
    request->data->label = label;
    request->data->protocol = protocol;
    std::string json(sctpStreamParameters.dump().c_str());
    DLOG("rtpCapabilities: {}", json);
    if (json.empty()) {
        return "";
    }
    std::string err;
    auto stcpsp = fromJsonString<signaling::ProduceDataRequest::SCTPStreamParameters>(json, err);
    if (!err.empty()) {
        DLOG("parse response failed: {}", err);
        return "";
    }
    request->data->sctpStreamParameters = *stcpsp;
    _mediasoupApi->produceData(request, [wself = weak_from_this(), &promise](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::ProduceDataResponse> response){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            promise.set_value("");
            return;
        }
        if (errorCode != 0) {
            DLOG("produceData failed, error code: {}, error info: {}", errorCode, errorInfo);
            promise.set_value("");
            return;
        }
        if (!response || !response->ok) {
            DLOG("response is null or response->ok == false");
            promise.set_value("");
            return;
        }
        promise.set_value(response->data->id.value_or(""));
    });

    return future.get();
}

void RoomClient::onOpened()
{
    _mediasoupThread->PostTask(RTC_FROM_HERE, [wself = weak_from_this()](){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }
        self->getRouterRtpCapabilities();
    });
}

void RoomClient::onClosed()
{
    _mediasoupThread->PostTask(RTC_FROM_HERE, [wself = weak_from_this()](){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }
        self->_state = RoomState::CLOSED;
        self->onRoomStateChanged(self->_state);
    });
}

void RoomClient::onActiveSpeaker(std::shared_ptr<signaling::ActiveSpeakerNotification> notification)
{
    if (!notification) {
        return;
    }
    if (_peerId != notification->data->peerId.value_or("")) {
        return;
    }
    _volume = notification->data->volume.value_or(0);

    UniversalObservable<IRoomClientObserver>::notifyObservers([wself = weak_from_this(), volume = _volume](const auto& observer) {
        auto self = wself.lock();
        if (!self) {
            return;
        }
        observer->onLocalActiveSpeaker(volume);
    });
}

void RoomClient::onLocalAudioStateChanged(bool enabled, bool muted)
{
    UniversalObservable<IRoomClientObserver>::notifyObservers([wself = weak_from_this(), enabled, muted](const auto& observer) {
        auto self = wself.lock();
        if (!self) {
            return;
        }
        observer->onLocalAudioStateChanged(enabled, muted);
    });
}

void RoomClient::onLocalVideoStateChanged(bool enabled)
{
    UniversalObservable<IRoomClientObserver>::notifyObservers([wself = weak_from_this(), enabled](const auto& observer) {
        auto self = wself.lock();
        if (!self) {
            return;
        }
        observer->onLocalVideoStateChanged(enabled);
    });
}

void RoomClient::onCreateLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    UniversalObservable<IRoomClientObserver>::notifyObservers([wself = weak_from_this(), tid, track](const auto& observer) {
        auto self = wself.lock();
        if (!self) {
            return;
        }
        observer->onCreateLocalVideoTrack(tid, track);
    });
}

void RoomClient::onRemoveLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    UniversalObservable<IRoomClientObserver>::notifyObservers([wself = weak_from_this(), tid, track](const auto& observer) {
        auto self = wself.lock();
        if (!self) {
            return;
        }
        observer->onRemoveLocalVideoTrack(tid, track);
    });
}

}
