/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

#include "websocket_transport.h"
#include <iostream>
#include "logger/spd_logger.h"
#include "json/serialization.hpp"
#include "i_connection_observer.h"
#include "websocket_endpoint.h"
#include "i_transport_observer.h"

namespace vi {
WebsocketTransport::WebsocketTransport(rtc::Thread* thread)
    : _thread(thread)
{

}

WebsocketTransport::~WebsocketTransport()
{
    DLOG("~WebsocketTransport()");
}

void WebsocketTransport::init()
{

}

void WebsocketTransport::destroy()
{

}

bool WebsocketTransport::isValid()
{
    if (_websocket && _connectionId != -1) {
        return true;
    }
    return false;
}

// ITransport
void WebsocketTransport::addObserver(std::shared_ptr<ITransportObserver> observer)
{
    UniversalObservable<ITransportObserver>::addWeakObserver(observer, _thread);
}

void WebsocketTransport::removeObserver(std::shared_ptr<ITransportObserver> observer)
{
    UniversalObservable<ITransportObserver>::removeObserver(observer);
}

void WebsocketTransport::connect(const std::string& url, const std::string& subprotocol)
{
    _url = url;

    _websocket = std::make_shared<WebsocketEndpoint>();

    if (_websocket) {
        _connectionId = _websocket->connect(_url, shared_from_this(), subprotocol);
    }
}

void WebsocketTransport::disconnect()
{
    if (isValid()) {
        _websocket->close(_connectionId, websocketpp::close::status::normal, "");
        _connectionId = -1;
        _websocket = nullptr;
    }
}

void WebsocketTransport::send(const std::string& text)
{
    if (!text.empty()) {
        _websocket->sendText(_connectionId, text);
        DLOG("sendText: {}", text);
    }
}

void WebsocketTransport::send(const std::vector<uint8_t>& data)
{
    if (!data.empty()) {
        _websocket->sendBinary(_connectionId, data);
    }
}

// IConnectionObserver
void WebsocketTransport::onOpen()
{
    DLOG("opened");

    UniversalObservable<ITransportObserver>::notifyObservers([wself = weak_from_this()](const auto& observer) {
        if (auto self = wself.lock()) {
            observer->onOpened();
        }
    });
}

void WebsocketTransport::onFail(int errorCode, const std::string& reason)
{
    DLOG("errorCode = {}, reason = {}", errorCode, reason.c_str());

    UniversalObservable<ITransportObserver>::notifyObservers([wself = weak_from_this(), errorCode, reason](const auto& observer) {
        if (auto self = wself.lock()) {
            observer->onFailed(errorCode, reason);
        }
    });
}

void WebsocketTransport::onClose(int closeCode, const std::string& reason)
{
    DLOG("errorCode = {}, reaseon = {}", closeCode, reason.c_str());

    UniversalObservable<ITransportObserver>::notifyObservers([wself = weak_from_this()](const auto& observer) {
        if (auto self = wself.lock()) {
            observer->onClosed();
        }
    });
}

bool WebsocketTransport::onValidate()
{
    DLOG("validate");
    return true;
}

void WebsocketTransport::onTextMessage(const std::string& json)
{
    DLOG("json = {}", json.c_str());
    UniversalObservable<ITransportObserver>::notifyObservers([wself = WebsocketTransport::weak_from_this(), msg = json](const auto& observer) {
        if (auto self = wself.lock()) {
            observer->onMessage(msg);
        }
    });
}

void WebsocketTransport::onBinaryMessage(const std::vector<uint8_t>& data)
{
    DLOG("data.size() = {}", data.size());
}

bool WebsocketTransport::onPing(const std::string& text)
{
    DLOG("text = {}", text.c_str());
    return true;
}

void WebsocketTransport::onPong(const std::string& text)
{
    DLOG("text = {}", text.c_str());
}

void WebsocketTransport::onPongTimeout(const std::string& text)
{
    DLOG("text = {}", text.c_str());
}
}
