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

namespace vi {

    class WebsocketTransport
        : public ITransport
        , public IConnectionObserver
        , public UniversalObservable<ITransportObserver>
        , public std::enable_shared_from_this<WebsocketTransport>
	{
	public:
        WebsocketTransport();

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
		std::string _url;

		int _connectionId = -1;

		std::shared_ptr<WebsocketEndpoint> _websocket;
	};
}
