#include "room_client.h"
#include "component_factory.h"
#include "service/signaling_client.h"
#include "service/mediasoup_api.h"
#include "signaling_client.h"
#include "logger/u_logger.h"
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
#include "Producer.hpp"
#include "DataProducer.hpp"
#include "Consumer.hpp"
#include "DataConsumer.hpp"
#include "media_controller.h"
#include "participant_controller.h"

namespace {

std::string getProtooUrl(const std::string& hostname, uint16_t port, const std::string& roomId, const std::string& peerId)
{
    std::stringstream sstr;
    sstr << "wss://" << hostname << ":" << port << "/?roomId=" << roomId << "&peerId=" << peerId;
    return sstr.str();
}

}

namespace vi {

RoomClient::RoomClient(std::weak_ptr<IComponentFactory> wcf)
    : _wcf(wcf)
{

}

RoomClient::~RoomClient()
{
    DLOG("~RoomClient()");
}

void RoomClient::init()
{
    if (!_signalingClient) {
        _signalingClient = std::make_shared<SignalingClient>();
        _signalingClient->init();
        _signalingClient->addObserver(shared_from_this());
    }

    if (_signalingClient && !_mediasoupApi) {
        _mediasoupApi = std::make_shared<MediasoupApi>(_signalingClient);
        _mediasoupApi->init();
    }

    if (!_mediaController.lock()) {
        auto mediaController = std::make_shared<MediaController>(_mediasoupApi, _sendTransport, _recvTransport, _peerConnectionFactory, _options);
        _mediaControllerProxy = MediaControllerProxy::create(mediaController, "mediasoup-client");
        _mediaControllerProxy->init();
        _mediaController = mediaController;
        _signalingClient->addObserver(mediaController);
    }

    if (!_participantController.lock()) {
        auto participantController = std::make_shared<ParticipantController>();
        _participantControllerProxy = ParticipantControllerProxy::create(participantController, "mediasoup-client");
        _participantControllerProxy->init();
        _participantController = participantController;
        _signalingClient->addObserver(participantController);
    }

    configWebrtc();
}

void RoomClient::destroy()
{
    if (_mediaControllerProxy) {
        if (_mediaControllerProxy->isVideoEnabled()) {
            _mediaControllerProxy->enableVideo(false);
        }
    }

    if (_participantControllerProxy) {
        _participantControllerProxy->destroy();
        _participantControllerProxy = nullptr;
    }

    if (_mediasoupApi) {
        _mediasoupApi->destroy();
        _mediasoupApi = nullptr;
    }

    if (_signalingClient) {
        _signalingClient->removeObserver(shared_from_this());
        _signalingClient->destroy();
        _signalingClient = nullptr;
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

    if (_mediaControllerProxy) {
        _mediaControllerProxy->destroy();
        _mediaControllerProxy = nullptr;
    }

    if (_peerConnectionFactory) {
        _peerConnectionFactory = nullptr;
    }
}

void RoomClient::addObserver(std::shared_ptr<IRoomClientObserver> observer)
{
    UniversalObservable<IRoomClientObserver>::addWeakObserver(observer, "mediasoup-client");
}

void RoomClient::removeObserver(std::shared_ptr<IRoomClientObserver> observer)
{
    UniversalObservable<IRoomClientObserver>::removeObserver(observer);
}

void RoomClient::join(const std::string& hostname, uint16_t port, const std::string& roomId, const std::string& displayName, std::shared_ptr<Options> options)
{
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

        UniversalObservable<IRoomClientObserver>::notifyObservers([](const auto& observer){
            observer->onRoomState(RoomState::CONNECTING);
        });
    }
    else {
        DLOG("_signalingClient is null");
    }
}

void RoomClient::leave()
{
    if (_signalingClient) {
        _signalingClient->disconnect();
    }
    else {
        DLOG("_signalingClient is null");
    }
}

std::shared_ptr<IMediaController> RoomClient::getMediaController()
{
    return _mediaControllerProxy;
}

std::shared_ptr<IParticipantController> RoomClient::getParticipantController()
{
    return _participantControllerProxy;
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
        TMgr->thread("mediasoup-client")->PostTask([wself, response](){
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
    _mediasoupDevice = std::make_shared<mediasoupclient::Device>();
    nlohmann::json rtpCapabilities = nlohmann::json::parse(response->data->toJsonStr());
    DLOG("rtpCapabilities: {}", response->data->toJsonStr());
    _mediasoupDevice->Load(rtpCapabilities);
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
        TMgr->thread("mediasoup-client")->PostTask([wself, producing, consuming, response](){
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

    _mediasoupApi->join(request, [wself = weak_from_this()](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::BasicResponse> response){
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

        self->UniversalObservable<IRoomClientObserver>::notifyObservers([](const auto& observer){
            observer->onRoomState(RoomState::CONNECTED);
        });
    });
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
    if (connectionState == "connected") {

    }
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

    _mediasoupApi->connectWebRtcTransport(request, [wself = weak_from_this(), &promise](int32_t errorCode, const std::string& errorInfo, std::shared_ptr<signaling::BasicResponse> response){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }
        if (errorCode != 0) {
            DLOG("connectWebRtcTransport failed, error code: {}, error info: {}", errorCode, errorInfo);
            return;
        }
        if (!response || !response->ok) {
            DLOG("response is null or response->ok == false");
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
            return;
        }
        if (errorCode != 0) {
            DLOG("produce failed, error code: {}, error info: {}", errorCode, errorInfo);
            return;
        }
        if (!response || !response->ok) {
            DLOG("response is null or response->ok == false");
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
            return;
        }
        if (errorCode != 0) {
            DLOG("produceData failed, error code: {}, error info: {}", errorCode, errorInfo);
            return;
        }
        if (!response || !response->ok) {
            DLOG("response is null or response->ok == false");
            return;
        }
        promise.set_value(response->data->id.value_or(""));
    });

    return future.get();
}

void RoomClient::configWebrtc()
{
    if (_peerConnectionFactory) {
        DLOG("already configured");
        return;
    }

    this->_networkThread = rtc::Thread::CreateWithSocketServer();
    this->_signalingThread = rtc::Thread::Create();
    this->_workerThread  = rtc::Thread::Create();

    this->_networkThread->SetName("network_thread", nullptr);
    this->_signalingThread->SetName("signaling_thread", nullptr);
    this->_workerThread->SetName("worker_thread", nullptr);

    if (!this->_networkThread->Start() || !this->_signalingThread->Start() || !this->_workerThread->Start())
    {
        ELOG("thread start errored");
    }

    this->_peerConnectionFactory = webrtc::CreatePeerConnectionFactory(this->_networkThread.get(),
                                                                       this->_workerThread.get(),
                                                                       this->_signalingThread.get(),
                                                                       nullptr /*default_adm*/,
                                                                       webrtc::CreateBuiltinAudioEncoderFactory(),
                                                                       webrtc::CreateBuiltinAudioDecoderFactory(),
                                                                       webrtc::CreateBuiltinVideoEncoderFactory(),
                                                                       webrtc::CreateBuiltinVideoDecoderFactory(),
                                                                       nullptr /*audio_mixer*/,
                                                                       nullptr /*audio_processing*/);

    _peerConnectionOptions.reset(new mediasoupclient::PeerConnection::Options());
    _peerConnectionOptions->config.set_dscp(true);
    _peerConnectionOptions->factory = _peerConnectionFactory;
}

void RoomClient::onOpened()
{
    TMgr->thread("mediasoup-client")->PostTask([wself = weak_from_this()](){
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
    TMgr->thread("mediasoup-client")->PostTask([wself = weak_from_this()](){
        auto self = wself.lock();
        if (!self) {
            DLOG("RoomClient is null");
            return;
        }
        // TODO:
    });
}

// Request from SFU
void RoomClient::onNewConsumer(std::shared_ptr<signaling::NewConsumerRequest> request)
{

}

void RoomClient::onNewDataConsumer(std::shared_ptr<signaling::NewDataConsumerRequest> request)
{

}

// Notification from SFU
void RoomClient::onProducerScore(std::shared_ptr<signaling::ProducerScoreNotification> notification)
{

}

void RoomClient::onConsumerScore(std::shared_ptr<signaling::ConsumerScoreNotification> notification)
{

}

void RoomClient::onNewPeer(std::shared_ptr<signaling::NewPeerNotification> notification)
{

}

void RoomClient::onPeerClosed(std::shared_ptr<signaling::PeerClosedNotification> notification)
{

}

void RoomClient::onPeerDisplayNameChanged(std::shared_ptr<signaling::PeerDisplayNameChangedNotification> notification)
{

}

void RoomClient::onConsumerPaused(std::shared_ptr<signaling::ConsumerPausedNotification> notification)
{

}

void RoomClient::onConsumerResumed(std::shared_ptr<signaling::ConsumerResumedNotification> notification)
{

}

void RoomClient::onConsumerClosed(std::shared_ptr<signaling::ConsumerClosedNotification> notification)
{

}

void RoomClient::onConsumerLayersChanged(std::shared_ptr<signaling::ConsumerLayersChangedNotification> notification)
{

}

void RoomClient::onDataConsumerClosed(std::shared_ptr<signaling::DataConsumerClosedNotification> notification)
{

}

void RoomClient::onDownlinkBwe(std::shared_ptr<signaling::DownlinkBweNotification> notification)
{

}

void RoomClient::onActiveSpeaker(std::shared_ptr<signaling::ActiveSpeakerNotification> notification)
{

}

}
