/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

#include "websocket_endpoint.h"
#include "i_connection_observer.h"
#include "logger/spd_logger.h"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/transport/asio/endpoint.hpp>
#include <websocketpp/transport/asio/security/tls.hpp>

#ifdef _MSC_VER
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
#endif

typedef std::shared_ptr<asio::ssl::context> context_ptr;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

namespace {
/// Verify that one of the subject alternative names matches the given hostname
bool verify_subject_alternative_name(const char * hostname, X509 * cert) {
    STACK_OF(GENERAL_NAME) * san_names = NULL;

    san_names = (STACK_OF(GENERAL_NAME) *) X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL);
    if (san_names == NULL) {
        return false;
    }

    int san_names_count = sk_GENERAL_NAME_num(san_names);

    bool result = false;

    for (int i = 0; i < san_names_count; i++) {
        const GENERAL_NAME * current_name = sk_GENERAL_NAME_value(san_names, i);

        if (current_name->type != GEN_DNS) {
            continue;
        }

        char const * dns_name = (char const *) ASN1_STRING_get0_data(current_name->d.dNSName);

        // Make sure there isn't an embedded NUL character in the DNS name
        if (ASN1_STRING_length(current_name->d.dNSName) != strlen(dns_name)) {
            break;
        }
        // Compare expected hostname with the CN
        result = (strcasecmp(hostname, dns_name) == 0);
    }
    sk_GENERAL_NAME_pop_free(san_names, GENERAL_NAME_free);

    return result;
}

/// Verify that the certificate common name matches the given hostname
bool verify_common_name(char const * hostname, X509 * cert) {
    // Find the position of the CN field in the Subject field of the certificate
    int common_name_loc = X509_NAME_get_index_by_NID(X509_get_subject_name(cert), NID_commonName, -1);
    if (common_name_loc < 0) {
        return false;
    }

    // Extract the CN field
    X509_NAME_ENTRY * common_name_entry = X509_NAME_get_entry(X509_get_subject_name(cert), common_name_loc);
    if (common_name_entry == NULL) {
        return false;
    }

    // Convert the CN field to a C string
    ASN1_STRING * common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
    if (common_name_asn1 == NULL) {
        return false;
    }

    char const * common_name_str = (char const *) ASN1_STRING_get0_data(common_name_asn1);

    // Make sure there isn't an embedded NUL character in the CN
    if (ASN1_STRING_length(common_name_asn1) != strlen(common_name_str)) {
        return false;
    }

    // Compare expected hostname with the CN
    return (strcasecmp(hostname, common_name_str) == 0);
}

/**
 * This code is derived from examples and documentation found ato00po
 * http://www.boost.org/doc/libs/1_61_0/doc/html/boost_asio/example/cpp03/ssl/client.cpp
 * and
 * https://github.com/iSECPartners/ssl-conservatory
 */
bool verify_certificate(const char * hostname, bool preverified, asio::ssl::verify_context& ctx) {
    // The verify callback can be used to check whether the certificate that is
    // being presented is valid for the peer. For example, RFC 2818 describes
    // the steps involved in doing this for HTTPS. Consult the OpenSSL
    // documentation for more details. Note that the callback is called once
    // for each certificate in the certificate chain, starting from the root
    // certificate authority.

    // Retrieve the depth of the current cert in the chain. 0 indicates the
    // actual server cert, upon which we will perform extra validation
    // (specifically, ensuring that the hostname matches. For other certs we
    // will use the 'preverified' flag from Asio, which incorporates a number of
    // non-implementation specific OpenSSL checking, such as the formatting of
    // certs and the trusted status based on the CA certs we imported earlier.
    int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());

    // if we are on the final cert and everything else checks out, ensure that
    // the hostname is present on the list of SANs or the common name (CN).
    if (depth == 0 && preverified) {
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());

        if (verify_subject_alternative_name(hostname, cert)) {
            return true;
        } else if (verify_common_name(hostname, cert)) {
            return true;
        } else {
            return false;
        }
    }

    return preverified;
}

/// TLS Initialization handler
/**
 * WebSocket++ core and the Asio Transport do not handle TLS context creation
 * and setup. This callback is provided so that the end user can set up their
 * TLS context using whatever settings make sense for their application.
 *
 * As Asio and OpenSSL do not provide great documentation for the very common
 * case of connect and actually perform basic verification of server certs this
 * example includes a basic implementation (using Asio and OpenSSL) of the
 * following reasonable default settings and verification steps:
 *
 * - Disable SSLv2 and SSLv3
 * - Load trusted CA certificates and verify the server cert is trusted.
 * - Verify that the hostname matches either the common name or one of the
 *   subject alternative names on the certificate.
 *
 * This is not meant to be an exhaustive reference implimentation of a perfect
 * TLS client, but rather a reasonable starting point for building a secure
 * TLS encrypted WebSocket client.
 *
 * If any TLS, Asio, or OpenSSL experts feel that these settings are poor
 * defaults or there are critically missing steps please open a GitHub issue
 * or drop a line on the project mailing list.
 *
 * Note the bundled CA cert ca-chain.cert.pem is the CA cert that signed the
 * cert bundled with echo_server_tls. You can use print_client_tls with this
 * CA cert to connect to echo_server_tls as long as you use /etc/hosts or
 * something equivilent to spoof one of the names on that cert
 * (websocketpp.org, for example).
 */
context_ptr on_tls_init(const char * hostname, websocketpp::connection_hdl) {
    context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

    try {
        ctx->set_options(asio::ssl::context::default_workarounds |
                         asio::ssl::context::no_sslv2 |
                         asio::ssl::context::no_sslv3 |
                         asio::ssl::context::single_dh_use);


        ctx->set_verify_mode(asio::ssl::verify_none);
        ctx->set_verify_callback(bind(&verify_certificate, hostname, ::_1, ::_2));

        // Here we load the CA certificates of all CA's that this client trusts.
        //ctx->load_verify_file("a-chain.cert.pem");
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return ctx;
}
}

namespace vi {
	WebsocketEndpoint::WebsocketEndpoint()
		: _nextId(0) {
		_endpoint.clear_access_channels(websocketpp::log::alevel::all);
		_endpoint.clear_error_channels(websocketpp::log::elevel::all);

        _endpoint.init_asio();
        //_endpoint.set_tls_init_handler(bind(&on_tls_init));
        _endpoint.set_tls_init_handler(bind(&on_tls_init, "", ::_1));
		_endpoint.start_perpetual();

        _thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&Client::run, &_endpoint);
	}

	WebsocketEndpoint::~WebsocketEndpoint() {
		_endpoint.stop_perpetual();

		for (ConnectionList::const_iterator it = _connectionList.begin(); it != _connectionList.end(); ++it) {
			if (it->second->getStatus() != "Open") {
				// Only close open connections
				continue;
			}

			DLOG("> Closing connection {}", it->second->getId());

			websocketpp::lib::error_code ec;
			_endpoint.close(it->second->getHdl(), websocketpp::close::status::going_away, "", ec);
			if (ec) {
				DLOG("> Error closing connection {}: {}", it->second->getId(), ec.message());
			}
		}

		if (_thread->joinable()) {
			_thread->join();
		}
	}

    int WebsocketEndpoint::connect(std::string const& uri, std::shared_ptr<IConnectionObserver> observer, const std::string& subprotocol) {
		websocketpp::lib::error_code ec;

        Client::connection_ptr con = _endpoint.get_connection(uri, ec);

		if (ec) {
			ELOG("> Connect initialization error: {}", ec.message());
			return -1;
		}

		if (!subprotocol.empty()) {
			con->add_subprotocol(subprotocol, ec);
			if (ec) {
				ELOG("> add subprotocol error: {}", ec.message());
				return -1;
			}
		}

		int newId = _nextId++;
        ConnectionMetadata::ptr metadataPtr = websocketpp::lib::make_shared<ConnectionMetadata>(newId, con->get_handle(), uri, observer);
		_connectionList[newId] = metadataPtr;

		con->set_open_handler(websocketpp::lib::bind(
			&ConnectionMetadata::onOpen,
			metadataPtr,
			&_endpoint,
			websocketpp::lib::placeholders::_1
		));
		con->set_fail_handler(websocketpp::lib::bind(
			&ConnectionMetadata::onFail,
			metadataPtr,
			&_endpoint,
			websocketpp::lib::placeholders::_1
		));
		con->set_close_handler(websocketpp::lib::bind(
			&ConnectionMetadata::onClose,
			metadataPtr,
			&_endpoint,
			websocketpp::lib::placeholders::_1
		));
		con->set_message_handler(websocketpp::lib::bind(
			&ConnectionMetadata::onMessage,
			metadataPtr,
			&_endpoint,
			websocketpp::lib::placeholders::_1,
			websocketpp::lib::placeholders::_2
		));

		con->set_ping_handler(websocketpp::lib::bind(
			&ConnectionMetadata::onPing,
			metadataPtr,
			&_endpoint,
			websocketpp::lib::placeholders::_1,
			websocketpp::lib::placeholders::_2
		));

		con->set_pong_handler(websocketpp::lib::bind(
			&ConnectionMetadata::onPong,
			metadataPtr,
			&_endpoint,
			websocketpp::lib::placeholders::_1,
			websocketpp::lib::placeholders::_2
		));

		con->set_pong_timeout_handler(websocketpp::lib::bind(
			&ConnectionMetadata::onPongTimeout,
			metadataPtr,
			&_endpoint,
			websocketpp::lib::placeholders::_1,
			websocketpp::lib::placeholders::_2
		));

		_endpoint.connect(con);

		return newId;
	}

	void WebsocketEndpoint::close(int id, websocketpp::close::status::value code, const std::string& reason) {
		websocketpp::lib::error_code ec;

		ConnectionList::iterator metadataIt = _connectionList.find(id);
		if (metadataIt == _connectionList.end()) {
			ELOG("> No connection found with id: {}", id);
			return;
		}

		_endpoint.close(metadataIt->second->getHdl(), code, reason, ec);
		if (ec) {
			ELOG("> Error initiating close: {}", ec.message());
		}
	}

	void WebsocketEndpoint::sendText(int id, const std::string& data) {
		websocketpp::lib::error_code ec;

		ConnectionList::iterator metadataIt = _connectionList.find(id);
		if (metadataIt == _connectionList.end()) {
			ELOG("> No connection found with id: {}", id);
			return;
		}

		_endpoint.send(metadataIt->second->getHdl(), data, websocketpp::frame::opcode::text, ec);
		if (ec) {
			ELOG("> Error sending text message: {}", ec.message());
			return;
		}
	}

	void WebsocketEndpoint::sendBinary(int id, const std::vector<uint8_t>& data)
	{
		websocketpp::lib::error_code ec;

		ConnectionList::iterator metadataIt = _connectionList.find(id);
		if (metadataIt == _connectionList.end()) {
			ELOG("> No connection found with id: {}", id);
			return;
		}

		_endpoint.send(metadataIt->second->getHdl(), data.data(), data.size(), websocketpp::frame::opcode::binary, ec);
		if (ec) {
			ELOG("> Error sending binary message: {}", ec.message());
			return;
		}
	}

	void WebsocketEndpoint::sendPing(int id, const std::string& data) {
		websocketpp::lib::error_code ec;

		ConnectionList::iterator metadataIt = _connectionList.find(id);
		if (metadataIt == _connectionList.end()) {
			ELOG("> No connection found with id: {}", id);
			return;
		}

		_endpoint.send(metadataIt->second->getHdl(), data, websocketpp::frame::opcode::ping, ec);
		if (ec) {
			ELOG("> Error sending ping message: {}", ec.message());
			return;
		}
	}

	void WebsocketEndpoint::sendPong(int id, const std::string& data) {
		websocketpp::lib::error_code ec;

		ConnectionList::iterator metadataIt = _connectionList.find(id);
		if (metadataIt == _connectionList.end()) {
			ELOG("> No connection found with id: {}", id);
			return;
		}

		_endpoint.send(metadataIt->second->getHdl(), data, websocketpp::frame::opcode::pong, ec);
		if (ec) {
			ELOG("> Error sending pong message: {}", ec.message());
			return;
		}
	}

	ConnectionMetadata::ptr WebsocketEndpoint::getMetadata(int id) const {
		ConnectionList::const_iterator metadataIt = _connectionList.find(id);
		if (metadataIt == _connectionList.end()) {
			return ConnectionMetadata::ptr();
		}
		else {
			return metadataIt->second;
		}
	}
}
