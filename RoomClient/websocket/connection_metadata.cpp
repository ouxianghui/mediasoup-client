/**
 * This file is part of janus_client project.
 * Author:    Jackie Ou
 * Created:   2020-10-01
 **/

#include "connection_metadata.h"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/endpoint.hpp>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "logger/u_logger.h"

namespace vi {
    ConnectionMetadata::ConnectionMetadata(int id, websocketpp::connection_hdl hdl, const std::string& uri, std::shared_ptr<IConnectionObserver> observer)
		: _id(id)
		, _hdl(hdl)
		, _status("Connecting")
		, _uri(uri)
		, _server("N/A")
        , _observer(observer)
	{}

    void ConnectionMetadata::onOpen(Client* c, websocketpp::connection_hdl hdl) {
		_status = "Open";

        Client::connection_ptr con = c->get_con_from_hdl(hdl);
		_server = con->get_response_header("Server");
        if (auto observer = _observer.lock()) {
            observer->onOpen();
		}
	}

    void ConnectionMetadata::onFail(Client* c, websocketpp::connection_hdl hdl) {
		_status = "Failed";

        Client::connection_ptr con = c->get_con_from_hdl(hdl);
		_server = con->get_response_header("Server");
		_errorReason = con->get_ec().message(); 
		int errorCode = con->get_ec().value();
        if (auto observer = _observer.lock()) {
            observer->onFail(errorCode, _errorReason);
		}
	}

    void ConnectionMetadata::onClose(Client* c, websocketpp::connection_hdl hdl) {
		_status = "Closed";
        Client::connection_ptr con = c->get_con_from_hdl(hdl);
		std::stringstream s;
		s << "close code: " << con->get_remote_close_code() << " (" << websocketpp::close::status::get_string(con->get_remote_close_code()) << "), close reason: " << con->get_remote_close_reason();
		_errorReason = s.str();

		int closeCode = con->get_remote_close_code();
        if (auto observer = _observer.lock()) {
            observer->onClose(closeCode, _errorReason);
		}
	}

    bool ConnectionMetadata::onValidate(Client* c, websocketpp::connection_hdl hdl)
	{
        if (auto observer = _observer.lock()) {
            observer->onValidate();
		}
		return true;
	}

    void ConnectionMetadata::onMessage(Client* c, websocketpp::connection_hdl, Client::message_ptr msg) {
        if (auto observer = _observer.lock()) {
			if (msg->get_opcode() == websocketpp::frame::opcode::text) {
				//DLOG("> received text message: {}", msg->get_payload());
                observer->onTextMessage(msg->get_payload());
			} else if (msg->get_opcode() == websocketpp::frame::opcode::binary) {
				//DLOG("> received binary message {}", websocketpp::utility::to_hex(msg->get_payload()));
				std::vector<uint8_t> data(msg->get_payload().begin(), msg->get_payload().end());
                observer->onBinaryMessage(data);
			}
		}
	}

    bool ConnectionMetadata::onPing(Client* c, websocketpp::connection_hdl, std::string msg)
	{
        if (auto observer = _observer.lock()) {
            observer->onPing(msg);
		}
		return true;
	}

    void ConnectionMetadata::onPong(Client* c, websocketpp::connection_hdl, std::string msg)
	{
        if (auto observer = _observer.lock()) {
            observer->onPong(msg);
		}
	}

    void  ConnectionMetadata::onPongTimeout(Client* c, websocketpp::connection_hdl, std::string msg)
	{
        if (auto observer = _observer.lock()) {
            observer->onPongTimeout(msg);
		}
	}

	websocketpp::connection_hdl ConnectionMetadata::getHdl() const {
		return _hdl;
	}

	int ConnectionMetadata::getId() const {
		return _id;
	}

	std::string ConnectionMetadata::getStatus() const {
		return _status;
	}

	std::ostream & operator<< (std::ostream& out, ConnectionMetadata const& data) {
		out << "> URI: " << data._uri << "\n"
			<< "> Status: " << data._status << "\n"
			<< "> Remote Server: " << (data._server.empty() ? "None Specified" : data._server) << "\n"
			<< "> Error/close reason: " << (data._errorReason.empty() ? "N/A" : data._errorReason) << "\n";
		return out;
	}

}
