/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#include "signaling_client.h"
#include "component_factory.h"
#include "logger/spd_logger.h"
#include "signaling_models.h"
#include "websocket/i_transport_observer.h"
#include "websocket/websocket_request.h"
#include "websocket/websocket_transport.h"
#include "websocket/tls_websocket_endpoint.h"
#include "rtc_base/thread.h"

namespace vi {

SignalingClient::SignalingClient(rtc::Thread* thread)
    : _thread(thread)
{
    _transport = std::make_shared<WebsocketTransport<TLSWebsocketEndpoint>>(_thread);
}

SignalingClient::~SignalingClient()
{
    clearRequests();
    DLOG("~SignalingClient()");
}

void SignalingClient::init()
{
    clearRequests();
    _transport->init();
    _transport->addObserver(shared_from_this());
}

void SignalingClient::destroy()
{
    UniversalObservable<ISignalingEventHandler>::clearObserver();
    clearRequests();
    _transport->removeObserver(shared_from_this());
}

void SignalingClient::addObserver(std::shared_ptr<ISignalingEventHandler> observer)
{
    UniversalObservable<ISignalingEventHandler>::addWeakObserver(observer, _thread);
}

void SignalingClient::removeObserver(std::shared_ptr<ISignalingEventHandler> observer)
{
    UniversalObservable<ISignalingEventHandler>::removeObserver(observer);
}

void SignalingClient::connect(const std::string& url, const std::string& subprotocol)
{
    _transport->connect(url, subprotocol);
}

void SignalingClient::disconnect()
{
    _transport->disconnect();
}

void SignalingClient::send(const std::string& text, int64_t transcation, SuccessCallback scb, FailureCallback fcb)
{
    if (!text.empty()) {
        uint32_t timeout = 1500 * (15 + (0.1 * _requestMap.size()));
        auto request = std::make_shared<vi::WebsocketRequest>(transcation, timeout);
        request->setText(text);
        request->setResolveCallback(scb);
        request->setRejectCallback(fcb);
        {
            std::lock_guard<std::mutex> locker(_requestMutex);
            _requestMap[request->id()] = request;
        }
        _transport->send(request->text());
        request->ticktock();
    }
}

void SignalingClient::send(const std::vector<uint8_t>& data, int64_t transcation, SuccessCallback scb, FailureCallback fcb)
{
    if (!data.empty()) {
        uint32_t timeout = 1500 * (15 + (0.1 * _requestMap.size()));
        auto request = std::make_shared<vi::WebsocketRequest>(transcation, timeout);
        request->setData(data);
        request->setResolveCallback(scb);
        request->setRejectCallback(fcb);
        {
            std::lock_guard<std::mutex> locker(_requestMutex);
            _requestMap[request->id()] = request;
        }
        _transport->send(request->data());
        request->ticktock();
    }
}

void SignalingClient::clearRequests()
{
    std::lock_guard<std::mutex> locker(_requestMutex);
    _requestMap.clear();
}

void SignalingClient::onOpened()
{
    UniversalObservable<ISignalingEventHandler>::notifyObservers([](const auto& observer){
        observer->onOpened();
    });
}

void SignalingClient::onClosed()
{
    std::lock_guard<std::mutex> locker(_requestMutex);
    for (const auto& it : _requestMap) {
        it.second->close();
    }

    _requestMap.clear();

    UniversalObservable<ISignalingEventHandler>::notifyObservers([](const auto& observer){
        observer->onClosed();
    });
}

void SignalingClient::onFailed(int /*errorCode*/, const std::string& /*reason*/)
{
    UniversalObservable<ISignalingEventHandler>::notifyObservers([](const auto& observer){
        observer->onClosed();
    });
}

void SignalingClient::onMessage(const std::string& json)
{
    std::string err;
    auto model = fromJsonString<signaling::MessageType>(json, err);
    if (!err.empty()) {
        DLOG("parse response failed: {}", err);
        return;
    }

    if (model->notification.value_or(false)) {
        handleNotification(json);
    }
    else if (model->response.value_or(false)) {
        handleResponse(json);
    }
    else if (model->request.value_or(false)) {
        handleRequest(json);
    }
}

void SignalingClient::handleRequest(const std::string& json)
{
    if (json.empty()) {
        return;
    }

    std::string err;
    auto header = fromJsonString<signaling::RequestHeader>(json, err);
    if (!err.empty()) {
        DLOG("parse response failed: {}", err);
        return;
    }

    auto method = header->method.value_or("");

    if (method == "newConsumer") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer){
            std::string err;
            auto request = fromJsonString<signaling::NewConsumerRequest>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onNewConsumer(request);
        });
    }
    else if (method == "newDataConsumer") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer){
            std::string err;
            auto request = fromJsonString<signaling::NewDataConsumerRequest>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onNewDataConsumer(request);
        });
    }
}

void SignalingClient::handleResponse(const std::string& json)
{
    if (_thread) {
        _thread->PostTask([wself = weak_from_this(), json]() {
            if (auto self = wself.lock()) {
                std::string err;
                auto header = fromJsonString<signaling::ResponseHeader>(json, err);
                if (!err.empty()) {
                    DLOG("parse response failed: {}", err);
                    return;
                }
                if (header && header->id.value_or(-1) != -1)  {
                    int64_t id = header->id.value();
                    std::shared_ptr<WebsocketRequest> request;
                    {
                        std::lock_guard<std::mutex> locker(self->_requestMutex);
                        if (self->_requestMap.find(id) != self->_requestMap.end()) {
                            request = self->_requestMap[id];
                            self->_requestMap.erase(id);
                        }
                    }
                    if (request) {
                        request->resolve(json);
                    }
                }
            }
        });
    }
}

void SignalingClient::handleNotification(const std::string& json)
{
    if (json.empty()) {
        return;
    }

    std::string err;
    auto header = fromJsonString<signaling::NotificationHeader>(json, err);
    if (!err.empty()) {
        DLOG("parse response failed: {}", err);
        return;
    }

    auto method = header->method.value_or("");

    if (method == "producerScore") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::ProducerScoreNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onProducerScore(notification);
        });
    }
    else if (method == "newPeer") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::NewPeerNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onNewPeer(notification);
        });
    }
    else if (method == "peerClosed") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::PeerClosedNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onPeerClosed(notification);
        });
    }
    else if (method == "peerDisplayNameChanged") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::PeerDisplayNameChangedNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onPeerDisplayNameChanged(notification);
        });
    }
    else if (method == "downlinkBwe") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::DownlinkBweNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onDownlinkBwe(notification);
        });
    }
    else if (method == "consumerClosed") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::ConsumerClosedNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onConsumerClosed(notification);
        });
    }
    else if (method == "consumerPaused") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::ConsumerPausedNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onConsumerPaused(notification);
        });
    }
    else if (method == "consumerResumed") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::ConsumerResumedNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onConsumerResumed(notification);
        });
    }
    else if (method == "consumerLayersChanged") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::ConsumerLayersChangedNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onConsumerLayersChanged(notification);
        });
    }
    else if (method == "consumerScore") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::ConsumerScoreNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onConsumerScore(notification);
        });
    }
    else if (method == "dataConsumerClosed") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::DataConsumerClosedNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onDataConsumerClosed(notification);
        });
    }
    else if (method == "activeSpeaker") {
        UniversalObservable<ISignalingEventHandler>::notifyObservers([json](const auto& observer) {
            std::string err;
            auto notification = fromJsonString<signaling::ActiveSpeakerNotification>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            observer->onActiveSpeaker(notification);
        });
    }
}

}


