/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include "connection_metadata.h"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <string>
#include <vector>

namespace vi {
	
	using TLSClient = websocketpp::client<websocketpp::config::asio_tls_client>;

	class TLSWebsocketEndpoint {
	public:
		TLSWebsocketEndpoint();

		~TLSWebsocketEndpoint();

        int connect(std::string const& uri, std::shared_ptr<IConnectionObserver> observer, const std::string& subprotocol = "");

		void close(int id, websocketpp::close::status::value code, const std::string& reason);

		void sendText(int id, const std::string& data);

		void sendBinary(int id, const std::vector<uint8_t>& data);

		void sendPing(int id, const std::string& data);

		void sendPong(int id, const std::string& data);

		ConnectionMetadata<TLSClient>::ptr getMetadata(int id) const;

	private:
		typedef std::map<int, ConnectionMetadata<TLSClient>::ptr> ConnectionList;

        TLSClient _endpoint;
		websocketpp::lib::shared_ptr<websocketpp::lib::thread> _thread;

		ConnectionList _connectionList;
		int _nextId;
	};
}

