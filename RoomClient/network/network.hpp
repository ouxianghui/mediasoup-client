#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <chrono>

#define DefaultTimeoutInterval (60 * 1000)

namespace vi {

enum class NetworkMethod : int
{
    HEAD = 0,
    GET,
    PUT,
    POST,
    DELETE_RESOURCE
};

enum class RequestResult : int
{
    SUCCESS,
    FAILED
};

class ContentType
{
public:
    static std::string text() { return "text/plain"; }
    static std::string json() { return "application/json"; }
    static std::string xml() { return "application/xml"; }
    static std::string multipartMixed() { return "multipart/mixed"; }
    static std::string xWwwFormUrlEncoded() { return "application/x-www-form-urlencoded;charset=UTF-8"; }
    static std::string compression() { return "gzip,deflate"; }
};

class HeaderType
{
public:
    static std::string contentType() { return "Content-Type"; }
    static std::string accept() { return "Accept"; }
    static std::string accpetLanaguage() { return "Accept-Language"; }
    static std::string host() { return "Host"; }
    static std::string userAgent() { return "User-Agent"; }
    static std::string authorization() { return "Authorization"; }
    static std::string ifNoneMatch() { return "If-None-Match"; }
    static std::string eTag() { return "Etag"; }
    static std::string acceptEncoding() { return "Accept-Encoding"; }
};

enum class NetworkErrorType : int {
    NoNetwork = -1,
    NoError = 0,
    ConnectionRefusedError = 1,
    RemoteHostClosedError = 2,
    HostNotFoundError = 3,
    TimeoutError = 4,
    OperationCanceledError = 5,
    SslHandshakeFailedError = 6,
    TemporaryNetworkFailureError = 7,
    NetworkSessionFailedError = 8,
    BackgroundRequestNotAllowedError = 9,
    TooManyRedirectsError = 10,
    InsecureRedirectError = 11,
    ProxyConnectionRefusedError = 101,
    ProxyConnectionClosedError = 102,
    ProxyNotFoundError = 103,
    ProxyTimeoutError = 104,
    ProxyAuthenticationRequiredError = 105,
    ContentAccessDenied = 201,
    ContentOperationNotPermittedError = 202,
    ContentNotFoundError = 203,
    AuthenticationRequiredError = 204,
    ContentReSendError = 205,
    ContentConflictError = 206,
    ContentGoneError = 207,
    InternalServerError = 401,
    OperationNotImplementedError = 402,
    ServiceUnavailableError = 403,
    ProtocolUnknownError = 301,
    ProtocolInvalidOperationError = 302,
    UnknownNetworkError = 99,
    UnknownProxyError = 199,
    UnknownContentError = 299,
    ProtocolFailure = 399,
    UnknownServerError = 499
};

enum class RequestRoute : int
{
    HTTP                = 1 << 0,
    MULTI_PART          = 1 << 1,
    UPLOAD              = 1 << 2,
    DOWNLOAD            = 1 << 3,
    IMAGE_DOWNLOAD      = 1 << 4,
    REQUEST_ROUTE_MAX = IMAGE_DOWNLOAD
};

enum class RequestPriority : int
{
    LOW,
    NORMAL,
    HIGH,
    SEPECIFIC,
};

struct NetworkActivityTime
{
    NetworkActivityTime()
        : createdTime()
        , executedTime()
        , finishedTime()
    {}

    int64_t createdTime;
    int64_t executedTime;
    int64_t finishedTime;
};

struct NetworkResponse final {
    int64_t code;
    std::unordered_map<std::string,std::string> header;
    RequestResult status;
    std::shared_ptr<std::vector<uint8_t>> data;
    std::string contentType;
    NetworkErrorType errorType;
    std::string errorDescription;

    NetworkResponse(int64_t code_,
                    std::unordered_map<std::string, std::string> header_,
                    RequestResult status_,
                    std::shared_ptr<std::vector<uint8_t>> data_,
                    std::string contentType_,
                    NetworkErrorType errorType_,
                    std::string errorDescription_)
        : code(std::move(code_))
        , header(std::move(header_))
        , status(std::move(status_))
        , data(std::move(data_))
        , contentType(std::move(contentType_))
        , errorType(std::move(errorType_))
        , errorDescription(std::move(errorDescription_))
    {}
};

struct ResponseError
{
public:
    ResponseError() {}
    ResponseError(int64_t code_, NetworkErrorType errorType_, const std::string& description_)
        : code(code_)
        , errorType(errorType_)
        , description(description_)
    {
    }

public:
    int64_t code;
    NetworkErrorType errorType;
    std::string description;
};

typedef std::function<void(const std::shared_ptr<NetworkResponse>)> RESPONSE_CALLBACK;

using UNIVERSAL_RESPONSE_CALLBACK = std::function<void(const std::shared_ptr<std::vector<uint8_t>>&,
                                                       const std::unordered_map<std::string, std::string>&,
                                                       const std::shared_ptr<ResponseError>&)>;

class NetworkRequest {
public:
    NetworkRequest(const std::string& pluginName, const std::string &host, const std::string &path, NetworkMethod method)
        : _pluginName(pluginName)
        , _host(host)
        , _path(path)
        , _method(method)
        , _header({})
        , _retryCount(0)
        , _timeout(DefaultTimeoutInterval)
        , _priority(RequestPriority::NORMAL)
        , _callback(nullptr)
        , _isCancelled(false)
        , _activityTime()
    {}

    int64_t requestId() const { return int64_t(this); }

    const std::string & host() const { return _host; }
    void setHost(const std::string & host) { _host = host; }

    int16_t port() const { return _port; }
    void setPort(int16_t port) { _port = port; }

    const std::string & path() const { return _path; }
    void setPath(const std::string & path) { _path = path; }

    const std::string & query() const { return _query; }
    void setQuery(const std::string & query) { _query = query; }

    NetworkMethod method() const { return _method; }
    void setMethod(NetworkMethod method) { _method = method; }

    const std::vector<uint8_t>& data() const { return _data; }
    void setData(const std::vector<uint8_t>& data) { _data.assign(data.begin(), data.end()); }

    RequestPriority priority() const { return _priority; }
    void setPriority(RequestPriority priority) { _priority = priority; }

    RequestRoute requestRoute() { return _requestRoute; }
    void setRequestRoute(RequestRoute requestRoute) { _requestRoute = requestRoute; }

    std::string pluginName() const { return _pluginName; }
    void setPluginName(const std::string& name) { _pluginName = name; }

    int64_t retryCount() const { return _retryCount; }
    void setRetryCount(int64_t count) { _retryCount = count; }

    int64_t timeout() const { return _timeout; }
    void setTimeout(int64_t timeout) { _timeout = timeout; }

    std::unordered_map<std::string, std::string>& header() { return _header; }
    void setHeader(const std::unordered_map<std::string, std::string>& header) { _header = std::move(header); }

    RESPONSE_CALLBACK callback() const { return _callback; }
    void setCallback(RESPONSE_CALLBACK cb) { _callback = std::move(cb); }

    bool isCancelled() const { return _isCancelled; }
    void setCancelled(bool isCancelled) { _isCancelled = isCancelled; }

    NetworkActivityTime & activityTime() { return _activityTime; }

private:
    std::string _host;
    int16_t _port = -1;
    std::string _path;
    std::string _query;
    NetworkMethod _method;
    std::string _pluginName;
    std::unordered_map<std::string, std::string> _header;
    std::vector<uint8_t> _data;
    int64_t _retryCount = 0;
    int64_t _timeout;
    RequestPriority _priority;
    RequestRoute _requestRoute;
    RESPONSE_CALLBACK _callback;
    std::atomic<bool> _isCancelled;
    NetworkActivityTime _activityTime;
};

class INetworkStatusListener {
public:
    virtual ~INetworkStatusListener() {}
    virtual void onNetworkStatusChanged(bool online) = 0;
};

class INetworkStatusDetector
{
public:
    virtual ~INetworkStatusDetector() {}
    virtual void addListener(const std::shared_ptr<INetworkStatusListener>& listener) = 0;
};

class INetworkRequestProducer
{
public:
    virtual ~INetworkRequestProducer() {}
    virtual const std::shared_ptr<NetworkRequest> produceRequest(RequestRoute route) = 0;
};

class INetworkRequestConsumer
{
public:
    virtual ~INetworkRequestConsumer() {}

    virtual void onNetworkRequestArrived() = 0;
    virtual bool canHandleRequest(RequestRoute route) = 0;
    virtual bool isAvailiable() = 0;
    virtual void cancelAll() = 0;
    virtual void cancelRequest(const std::shared_ptr<NetworkRequest>& request) = 0;
};

class INetworkCallback {
public:
    virtual ~INetworkCallback() {}

    virtual void onSuccess(int64_t requestID, const std::shared_ptr<NetworkResponse>& response) = 0;
    virtual void onFailure(int64_t requestID, const std::shared_ptr<NetworkResponse>& response) = 0;
    virtual void onProgress(int64_t requestID, int32_t max_size, int32_t current_pos) = 0;
};

class INetworkClient
{
public:
    virtual ~INetworkClient() {}

    virtual void request(const std::shared_ptr<NetworkRequest>& request, const std::shared_ptr<INetworkCallback>& callback) = 0;
    virtual void init() = 0;
    virtual void reset() = 0;
    virtual bool isNetworkReachable() = 0;
    virtual bool isAvailable() = 0;
    virtual void cancelRequest(const std::shared_ptr<NetworkRequest>& request) = 0;
};

class NetworkRequestExecutor;
class INetworkRequestExecutorListener
{
public:
    virtual ~INetworkRequestExecutorListener() {}
    virtual void onComplete(const std::shared_ptr<NetworkRequestExecutor>& executor) = 0;
};

class INetworkRequestHandler
{
public:
    virtual ~INetworkRequestHandler() {}

    virtual void addRequest(const std::shared_ptr<NetworkRequest>& request, bool isTail) = 0;
    virtual void cancelRequest(const std::shared_ptr<NetworkRequest>& request) = 0;
    virtual void cancelAll() = 0;

    virtual std::string pluginName() const = 0;
};

using VOID_CALLBACK = std::function<void(const std::shared_ptr<ResponseError>&)>;

using STRING_CALLBACK = std::function<void(const std::string&, const std::shared_ptr<ResponseError>&)>;

}

