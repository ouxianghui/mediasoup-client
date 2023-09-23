/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

#pragma once

#include <memory>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/endpoint.hpp>
#include "i_connection_observer.h"
#include "logger/spd_logger.h"

namespace vi {

	template<typename T, typename MsgPtr = T::message_ptr>
	class ConnectionMetadata {
	public:
		typedef websocketpp::lib::shared_ptr<ConnectionMetadata> ptr;

        ConnectionMetadata(int id, websocketpp::connection_hdl hdl, const std::string& uri, std::shared_ptr<IConnectionObserver> observer);

        void onOpen(T* c, websocketpp::connection_hdl hdl);

        void onFail(T* c, websocketpp::connection_hdl hdl);

        void onClose(T* c, websocketpp::connection_hdl hdl);

        bool onValidate(T* c, websocketpp::connection_hdl hdl);

        void onMessage(T* c, websocketpp::connection_hdl, MsgPtr msg);

        bool onPing(T* c, websocketpp::connection_hdl, std::string msg);

        void onPong(T* c, websocketpp::connection_hdl, std::string msg);

        void onPongTimeout(T* c, websocketpp::connection_hdl, std::string msg);

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

	template<typename T, typename MsgPtr>
	ConnectionMetadata<T, MsgPtr>::ConnectionMetadata(int id, websocketpp::connection_hdl hdl, const std::string& uri, std::shared_ptr<IConnectionObserver> observer)
		: _id(id)
		, _hdl(hdl)
		, _status("Connecting")
		, _uri(uri)
		, _server("N/A")
		, _observer(observer)
	{}

	template<typename T, typename MsgPtr>
	void ConnectionMetadata<T, MsgPtr>::onOpen(T* c, websocketpp::connection_hdl hdl) {
		_status = "Open";

		T::connection_ptr con = c->get_con_from_hdl(hdl);
		_server = con->get_response_header("Server");
		if (auto observer = _observer.lock()) {
			observer->onOpen();
		}
	}

	template<typename T, typename MsgPtr>
	void ConnectionMetadata<T, MsgPtr>::onFail(T* c, websocketpp::connection_hdl hdl) {
		_status = "Failed";

		T::connection_ptr con = c->get_con_from_hdl(hdl);
		_server = con->get_response_header("Server");
		_errorReason = con->get_ec().message();
		int errorCode = con->get_ec().value();
		if (auto observer = _observer.lock()) {
			observer->onFail(errorCode, _errorReason);
		}
	}

	template<typename T, typename MsgPtr>
	void ConnectionMetadata<T, MsgPtr>::onClose(T* c, websocketpp::connection_hdl hdl) {
		_status = "Closed";
		T::connection_ptr con = c->get_con_from_hdl(hdl);
		std::stringstream s;
		s << "close code: " << con->get_remote_close_code() << " (" << websocketpp::close::status::get_string(con->get_remote_close_code()) << "), close reason: " << con->get_remote_close_reason();
		_errorReason = s.str();

		int closeCode = con->get_remote_close_code();
		if (auto observer = _observer.lock()) {
			observer->onClose(closeCode, _errorReason);
		}
	}

	template<typename T, typename MsgPtr>
	bool ConnectionMetadata<T, MsgPtr>::onValidate(T* c, websocketpp::connection_hdl hdl)
	{
		if (auto observer = _observer.lock()) {
			observer->onValidate();
		}
		return true;
	}

	template<typename T, typename MsgPtr>
	void ConnectionMetadata<T, MsgPtr>::onMessage(T* c, websocketpp::connection_hdl, MsgPtr msg) {
		if (auto observer = _observer.lock()) {
			if (msg->get_opcode() == websocketpp::frame::opcode::text) {
				observer->onTextMessage(msg->get_payload());
			}
			else if (msg->get_opcode() == websocketpp::frame::opcode::binary) {
				std::vector<uint8_t> data(msg->get_payload().begin(), msg->get_payload().end());
				observer->onBinaryMessage(data);
			}
		}
	}

	template<typename T, typename MsgPtr>
	bool ConnectionMetadata<T, MsgPtr>::onPing(T* c, websocketpp::connection_hdl, std::string msg)
	{
		if (auto observer = _observer.lock()) {
			observer->onPing(msg);
		}
		return true;
	}

	template<typename T, typename MsgPtr>
	void ConnectionMetadata<T, MsgPtr>::onPong(T* c, websocketpp::connection_hdl, std::string msg)
	{
		if (auto observer = _observer.lock()) {
			observer->onPong(msg);
		}
	}

	template<typename T, typename MsgPtr>
	void ConnectionMetadata<T, MsgPtr>::onPongTimeout(T* c, websocketpp::connection_hdl, std::string msg)
	{
		if (auto observer = _observer.lock()) {
			observer->onPongTimeout(msg);
		}
	}

	template<typename T, typename MsgPtr>
	websocketpp::connection_hdl ConnectionMetadata<T, MsgPtr>::getHdl() const {
		return _hdl;
	}

	template<typename T, typename MsgPtr>
	int ConnectionMetadata<T, MsgPtr>::getId() const {
		return _id;
	}

	template<typename T, typename MsgPtr>
	std::string ConnectionMetadata<T, MsgPtr>::getStatus() const {
		return _status;
	}

	template<typename T, typename MsgPtr>
	std::ostream& operator<< (std::ostream& out, ConnectionMetadata<T, MsgPtr> const& data) {
		out << "> URI: " << data._uri << "\n"
			<< "> Status: " << data._status << "\n"
			<< "> Remote Server: " << (data._server.empty() ? "None Specified" : data._server) << "\n"
			<< "> Error/close reason: " << (data._errorReason.empty() ? "N/A" : data._errorReason) << "\n";
		return out;
	}
}


