/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

#pragma once

#include <memory>
#include <thread>
#include <unordered_map>
#include "i_transport.h"
#include "i_connection_observer.h"
#include "websocket_endpoint.h"
#include "utils/universal_observable.hpp"
#include "rtc_base/thread.h"
#include "logger/spd_logger.h"
#include "json/serialization.hpp"
#include "i_transport_observer.h"

namespace vi {

    template<typename T>
    class WebsocketTransport
        : public ITransport
        , public IConnectionObserver
        , public UniversalObservable<ITransportObserver>
        , public std::enable_shared_from_this<WebsocketTransport<T>>
	{
	public:
        WebsocketTransport(rtc::Thread* thread);

        ~WebsocketTransport() override;

		void init() override;

		void destroy() override;

        // ITransportor
        void addObserver(std::shared_ptr<ITransportObserver> observer) override;

        void removeObserver(std::shared_ptr<ITransportObserver> observer) override;

        void connect(const std::string& url, const std::string& subprotocol) override;

		void disconnect() override;

        void send(const std::string& text) override;

        void send(const std::vector<uint8_t>& data) override;

	protected:
        // IConnectionObserver implement
		void onOpen() override;

		void onFail(int errorCode, const std::string& reason) override;

		void onClose(int closeCode, const std::string& reason) override;

		bool onValidate() override;

		void onTextMessage(const std::string& text) override;

		void onBinaryMessage(const std::vector<uint8_t>& data) override;

		bool onPing(const std::string& text) override;

		void onPong(const std::string& text) override;

		void onPongTimeout(const std::string& text) override;

    protected:
		bool isValid();

    private:
		rtc::Thread* _thread;

		std::string _url;

		int _connectionId = -1;

		std::shared_ptr<T> _websocket;
	};

    template<typename T>
    WebsocketTransport<T>::WebsocketTransport(rtc::Thread* thread)
        : _thread(thread)
    {

    }

    template<typename T>
    WebsocketTransport<T>::~WebsocketTransport()
    {
        DLOG("~WebsocketTransport()");
    }

    template<typename T>
    void WebsocketTransport<T>::init()
    {

    }

    template<typename T>
    void WebsocketTransport<T>::destroy()
    {

    }

    template<typename T>
    bool WebsocketTransport<T>::isValid()
    {
        if (_websocket && _connectionId != -1) {
            return true;
        }
        return false;
    }

    // ITransport
    template<typename T>
    void WebsocketTransport<T>::addObserver(std::shared_ptr<ITransportObserver> observer)
    {
        UniversalObservable<ITransportObserver>::addWeakObserver(observer, _thread);
    }

    template<typename T>
    void WebsocketTransport<T>::removeObserver(std::shared_ptr<ITransportObserver> observer)
    {
        UniversalObservable<ITransportObserver>::removeObserver(observer);
    }

    template<typename T>
    void WebsocketTransport<T>::connect(const std::string& url, const std::string& subprotocol)
    {
        _url = url;

        _websocket = std::make_shared<T>();

        if (_websocket) {
            _connectionId = _websocket->connect(_url, shared_from_this(), subprotocol);
        }
    }

    template<typename T>
    void WebsocketTransport<T>::disconnect()
    {
        if (isValid()) {
            _websocket->close(_connectionId, websocketpp::close::status::normal, "");
            _connectionId = -1;
            _websocket = nullptr;
        }
    }

    template<typename T>
    void WebsocketTransport<T>::send(const std::string& text)
    {
        if (!text.empty()) {
            _websocket->sendText(_connectionId, text);
            DLOG("sendText: {}", text);
        }
    }

    template<typename T>
    void WebsocketTransport<T>::send(const std::vector<uint8_t>& data)
    {
        if (!data.empty()) {
            _websocket->sendBinary(_connectionId, data);
        }
    }

    // IConnectionObserver
    template<typename T>
    void WebsocketTransport<T>::onOpen()
    {
        DLOG("opened");

        UniversalObservable<ITransportObserver>::notifyObservers([wself = weak_from_this()](const auto& observer) {
            if (auto self = wself.lock()) {
                observer->onOpened();
            }
        });
    }

    template<typename T>
    void WebsocketTransport<T>::onFail(int errorCode, const std::string& reason)
    {
        DLOG("errorCode = {}, reason = {}", errorCode, reason.c_str());

        UniversalObservable<ITransportObserver>::notifyObservers([wself = weak_from_this(), errorCode, reason](const auto& observer) {
            if (auto self = wself.lock()) {
                observer->onFailed(errorCode, reason);
            }
        });
    }

    template<typename T>
    void WebsocketTransport<T>::onClose(int closeCode, const std::string& reason)
    {
        DLOG("errorCode = {}, reaseon = {}", closeCode, reason.c_str());

        UniversalObservable<ITransportObserver>::notifyObservers([wself = weak_from_this()](const auto& observer) {
            if (auto self = wself.lock()) {
                observer->onClosed();
            }
        });
    }

    template<typename T>
    bool WebsocketTransport<T>::onValidate()
    {
        DLOG("validate");
        return true;
    }

    template<typename T>
    void WebsocketTransport<T>::onTextMessage(const std::string& json)
    {
        DLOG("json = {}", json.c_str());
        UniversalObservable<ITransportObserver>::notifyObservers([wself = WebsocketTransport<T>::weak_from_this(), msg = json](const auto& observer) {
            if (auto self = wself.lock()) {
                observer->onMessage(msg);
            }
        });
    }

    template<typename T>
    void WebsocketTransport<T>::onBinaryMessage(const std::vector<uint8_t>& data)
    {
        DLOG("data.size() = {}", data.size());
    }

    template<typename T>
    bool WebsocketTransport<T>::onPing(const std::string& text)
    {
        DLOG("text = {}", text.c_str());
        return true;
    }

    template<typename T>
    void WebsocketTransport<T>::onPong(const std::string& text)
    {
        DLOG("text = {}", text.c_str());
    }

    template<typename T>
    void WebsocketTransport<T>::onPongTimeout(const std::string& text)
    {
        DLOG("text = {}", text.c_str());
    }
}
