#include "network_http_client.h"
#include "logger/spd_logger.h"
#include <cpr/cpr.h>

namespace {
using namespace vi;
NetworkErrorType convert(cpr::ErrorCode errorCode)
{
    NetworkErrorType errorType = NetworkErrorType::NoError;
    switch (errorCode) {
    case cpr::ErrorCode::OK:
        errorType = NetworkErrorType::NoError;
        break;
    case cpr::ErrorCode::CONNECTION_FAILURE:
        errorType = NetworkErrorType::ConnectionRefusedError;
        break;
    case cpr::ErrorCode::EMPTY_RESPONSE:
        errorType = NetworkErrorType::UnknownContentError;
        break;
    case cpr::ErrorCode::HOST_RESOLUTION_FAILURE:
        errorType = NetworkErrorType::HostNotFoundError;
        break;
    case cpr::ErrorCode::INTERNAL_ERROR:
        errorType = NetworkErrorType::InternalServerError;
        break;
    case cpr::ErrorCode::INVALID_URL_FORMAT:
        errorType = NetworkErrorType::UnknownContentError;
        break;
    case cpr::ErrorCode::NETWORK_RECEIVE_ERROR:
        errorType = NetworkErrorType::TemporaryNetworkFailureError;
        break;
    case cpr::ErrorCode::NETWORK_SEND_FAILURE:
        errorType = NetworkErrorType::TemporaryNetworkFailureError;
        break;
    case cpr::ErrorCode::OPERATION_TIMEDOUT:
        errorType = NetworkErrorType::TimeoutError;
        break;
    case cpr::ErrorCode::PROXY_RESOLUTION_FAILURE:
        errorType = NetworkErrorType::ProxyNotFoundError;
        break;
    case cpr::ErrorCode::SSL_CONNECT_ERROR:
        errorType = NetworkErrorType::ConnectionRefusedError;
        break;
    case cpr::ErrorCode::SSL_LOCAL_CERTIFICATE_ERROR:
        errorType = NetworkErrorType::SslHandshakeFailedError;
        break;
    case cpr::ErrorCode::SSL_REMOTE_CERTIFICATE_ERROR:
        errorType = NetworkErrorType::SslHandshakeFailedError;
        break;
    case cpr::ErrorCode::SSL_CACERT_ERROR:
        errorType = NetworkErrorType::SslHandshakeFailedError;
        break;
    case cpr::ErrorCode::GENERIC_SSL_ERROR:
        errorType = NetworkErrorType::SslHandshakeFailedError;
        break;
    case cpr::ErrorCode::UNSUPPORTED_PROTOCOL:
        errorType = NetworkErrorType::ProtocolInvalidOperationError;
        break;
    case cpr::ErrorCode::REQUEST_CANCELLED:
        errorType = NetworkErrorType::OperationCanceledError;
        break;
    case cpr::ErrorCode::TOO_MANY_REDIRECTS:
        errorType = NetworkErrorType::TooManyRedirectsError;
        break;
    case cpr::ErrorCode::UNKNOWN_ERROR:
        errorType = NetworkErrorType::UnknownServerError;
        break;
    default:
        break;
    }

    return errorType;
}
}

namespace vi {

NetworkHttpClient::NetworkHttpClient()
{

}

NetworkHttpClient::~NetworkHttpClient()
{

}

void NetworkHttpClient::init()
{
    
}

void NetworkHttpClient::reset()
{

}

void NetworkHttpClient::request(const std::shared_ptr<NetworkRequest>& request, const std::shared_ptr<INetworkCallback>& callback)
{
    DLOG("request id:{}", request->requestId());

    std::string url = request->host();
    if (request->port() != -1) {
        url += ":";
        url += std::to_string(request->port());
    }
    url += request->path();

    DLOG("request url:{}", url);

    if (request->method() == NetworkMethod::POST) {
        std::string body((char*)request->data().data(), request->data().size());
        cpr::PostCallback([wself = std::weak_ptr<NetworkHttpClient>(shared_from_this()), reqId = request->requestId(), callback](const cpr::Response& r) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            std::unordered_map<std::string, std::string> header;
            for (const auto& pair : r.header) {
                header[pair.first] = pair.second;
            }
            std::string contentType;
            if (header.find("Content-Type") != header.end()) {
                contentType = r.header.at("Content-Type");
            }
            auto data = std::make_shared<std::vector<uint8_t>>(r.text.begin(), r.text.end());
            std::shared_ptr<NetworkResponse> response = std::make_shared<NetworkResponse>(r.status_code,
                                                                                          header,
                                                                                          r.error.code == cpr::ErrorCode::OK ? RequestResult::SUCCESS : RequestResult::FAILED,
                                                                                          data,
                                                                                          contentType,
                                                                                          convert(r.error.code),
                                                                                          r.reason);

            self->handleResults(reqId, response, callback);
        },
                          cpr::Url{ url },
                          cpr::Body{ body },
                          cpr::Header{ { "Content-Type", "application/json" } },
                          cpr::VerifySsl{ true }
                          );
    }
    else if (request->method() == NetworkMethod::GET) {
        cpr::GetCallback([wself = std::weak_ptr<NetworkHttpClient>(shared_from_this()), reqId = request->requestId(), callback](const cpr::Response& r) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            std::unordered_map<std::string, std::string> header;
            for (const auto& pair : r.header) {
                header[pair.first] = pair.second;
            }
            std::string contentType;
            if (header.find("Content-Type") != header.end()) {
                contentType = r.header.at("Content-Type");
            }
            auto data = std::make_shared<std::vector<uint8_t>>(r.text.begin(), r.text.end());
            std::shared_ptr<NetworkResponse> response = std::make_shared<NetworkResponse>(r.status_code,
                                                                                          header,
                                                                                          r.error.code == cpr::ErrorCode::OK ? RequestResult::SUCCESS : RequestResult::FAILED,
                                                                                          data,
                                                                                          contentType,
                                                                                          convert(r.error.code),
                                                                                          r.reason);

            self->handleResults(reqId, response, callback);
        },
                         cpr::Url{ url }
                         );
    }
    else if (request->method() == NetworkMethod::PUT) {
        std::string payload((char*)request->data().data(), request->data().size());
        cpr::PutCallback([wself = std::weak_ptr<NetworkHttpClient>(shared_from_this()), reqId = request->requestId(), callback](const cpr::Response& r) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            std::unordered_map<std::string, std::string> header;
            for (const auto& pair : r.header) {
                header[pair.first] = pair.second;
            }
            std::string contentType;
            if (header.find("Content-Type") != header.end()) {
                contentType = r.header.at("Content-Type");
            }
            auto data = std::make_shared<std::vector<uint8_t>>(r.text.begin(), r.text.end());
            std::shared_ptr<NetworkResponse> response = std::make_shared<NetworkResponse>(r.status_code,
                                                                                          header,
                                                                                          r.error.code == cpr::ErrorCode::OK ? RequestResult::SUCCESS : RequestResult::FAILED,
                                                                                          data,
                                                                                          contentType,
                                                                                          convert(r.error.code),
                                                                                          r.reason);

            self->handleResults(reqId, response, callback);
        },
                         cpr::Url{ url },
                         cpr::Payload{ } // TODO: key:value
                         );
    }
    else if (request->method() == NetworkMethod::DELETE_RESOURCE) {
        cpr::DeleteCallback([wself = std::weak_ptr<NetworkHttpClient>(shared_from_this()), reqId = request->requestId(), callback](const cpr::Response& r) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            std::unordered_map<std::string, std::string> header;
            for (const auto& pair : r.header) {
                header[pair.first] = pair.second;
            }
            std::string contentType;
            if (header.find("Content-Type") != header.end()) {
                contentType = r.header.at("Content-Type");
            }
            auto data = std::make_shared<std::vector<uint8_t>>(r.text.begin(), r.text.end());
            std::shared_ptr<NetworkResponse> response = std::make_shared<NetworkResponse>(r.status_code,
                                                                                          header,
                                                                                          r.error.code == cpr::ErrorCode::OK ? RequestResult::SUCCESS : RequestResult::FAILED,
                                                                                          data,
                                                                                          contentType,
                                                                                          convert(r.error.code),
                                                                                          r.reason);

            self->handleResults(reqId, response, callback);
        },
                            cpr::Url{ url }
                            );
    }
    else if (request->method() == NetworkMethod::HEAD) {
        cpr::HeadCallback([wself = std::weak_ptr<NetworkHttpClient>(shared_from_this()), reqId = request->requestId(), callback](const cpr::Response& r) {
            auto self = wself.lock();
            if (!self) {
                return;
            }
            std::unordered_map<std::string, std::string> header;
            for (const auto& pair : r.header) {
                header[pair.first] = pair.second;
            }
            std::string contentType;
            if (header.find("Content-Type") != header.end()) {
                contentType = r.header.at("Content-Type");
            }
            auto data = std::make_shared<std::vector<uint8_t>>(r.text.begin(), r.text.end());
            std::shared_ptr<NetworkResponse> response = std::make_shared<NetworkResponse>(r.status_code,
                                                                                          header,
                                                                                          r.error.code == cpr::ErrorCode::OK ? RequestResult::SUCCESS : RequestResult::FAILED,
                                                                                          data,
                                                                                          contentType,
                                                                                          convert(r.error.code),
                                                                                          r.reason);

            self->handleResults(reqId, response, callback);
        },
                          cpr::Url{ url }
                          );
    }
}

bool NetworkHttpClient::isNetworkReachable()
{
    // return NetworkStatusDetector::instance()->isOnline();
    return true;
}

bool NetworkHttpClient::isAvailable()
{
    return true;
}

void NetworkHttpClient::cancelRequest(const std::shared_ptr<NetworkRequest>& /*request*/)
{

}

void NetworkHttpClient::handleResults(int64_t requestID, std::shared_ptr<NetworkResponse> response, std::shared_ptr<INetworkCallback> callback)
{
    if (callback) {
        if (response->status == RequestResult::SUCCESS) {
            callback->onSuccess(requestID, response);
        } else {
            callback->onFailure(requestID, response);
        }
    }
}

}
