/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

#pragma once

#include <memory>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include "i_connection_observer.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> Client;

namespace vi {

	class ConnectionMetadata {
	public:
		typedef websocketpp::lib::shared_ptr<ConnectionMetadata> ptr;

        ConnectionMetadata(int id, websocketpp::connection_hdl hdl, const std::string& uri, std::shared_ptr<IConnectionObserver> observer);

        void onOpen(Client* c, websocketpp::connection_hdl hdl);

        void onFail(Client* c, websocketpp::connection_hdl hdl);

        void onClose(Client* c, websocketpp::connection_hdl hdl);

        bool onValidate(Client* c, websocketpp::connection_hdl hdl);

        void onMessage(Client* c, websocketpp::connection_hdl, Client::message_ptr msg);

        bool onPing(Client* c, websocketpp::connection_hdl, std::string msg);

        void onPong(Client* c, websocketpp::connection_hdl, std::string msg);

        void onPongTimeout(Client* c, websocketpp::connection_hdl, std::string msg);

		websocketpp::connection_hdl getHdl() const;

		int getId() const;

		std::string getStatus() const;

		friend std::ostream & operator<< (std::ostream& out, ConnectionMetadata const& data);
	private:
		int _id;
		websocketpp::connection_hdl _hdl;
		std::string _status;
		std::string _uri;
		std::string _server;
		std::string _errorReason;
        std::weak_ptr<IConnectionObserver> _observer;
	};

}


