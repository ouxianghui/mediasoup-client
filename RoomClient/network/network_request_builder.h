#pragma once

#include "network.hpp"

namespace vi {

class NetworkRequestBuilder : public std::enable_shared_from_this<NetworkRequestBuilder> {
public:
    NetworkRequestBuilder(const std::string& pluginName)
        : _pluginName(pluginName)
    {

    }

    NetworkRequestBuilder(const std::string& pluginName, const std::string& host, const std::string& path, NetworkMethod method)
        : _pluginName(pluginName)
        , _host(host)
        , _path(path)
        , _method(method)
    {

    }

    static std::shared_ptr<NetworkRequestBuilder> create(const std::string& pluginName)
    {
        auto builder = std::make_shared<NetworkRequestBuilder>(pluginName);
        return builder;
    }

    static std::shared_ptr<NetworkRequestBuilder> create(const std::string& pluginName, 
                                                         const std::string& host,
                                                         const std::string& path,
                                                         NetworkMethod method)
    {
        auto builder = std::make_shared<NetworkRequestBuilder>(pluginName, host, path, method);
        return builder;
    }

    std::shared_ptr<NetworkRequestBuilder> setHost(const std::string & host) {
        _host = host;
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setPort(int16_t port) {
        _port = port;
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setPath(const std::string & path) {
        _path = path;
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setQuery(const std::string & query) {
        _query = query;
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setMethod(NetworkMethod method) {
        _method = method;
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setRequestRoute(RequestRoute requestRoute) {
        _requestRoute = requestRoute;
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setData(const std::vector<uint8_t>& data) {
        _data.assign(data.begin(), data.end());
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setPriority(RequestPriority priority) {
        _priority = priority;
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setRetryCount(int64_t count) {
        _retryCount = count;
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setTimeout(int64_t timeout) {
        _timeout = timeout;
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setHeader(const std::unordered_map<std::string, std::string>& header) {
        _header = std::move(header);
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequestBuilder> setCallback(UNIVERSAL_RESPONSE_CALLBACK cb) {
        _callback = transformCallback(cb);
        return std::enable_shared_from_this<NetworkRequestBuilder>::shared_from_this();
    }

    std::shared_ptr<NetworkRequest> build() {
        std::shared_ptr<NetworkRequest> request = std::make_shared<NetworkRequest>(_pluginName, _host, _path, _method);
        request->setPort(_port);
        request->setQuery(_query);
        request->setData(_data);
        request->setTimeout(_timeout);
        request->setRetryCount(_retryCount);
        request->setCallback(_callback);
        request->setRequestRoute(_requestRoute);

        if (!_contentType.empty()) {
            _header.insert({HeaderType::contentType(), _contentType});
        }

        request->setHeader(_header);
        request->activityTime().createdTime = std::chrono::steady_clock::now().time_since_epoch().count();

        return request;
    }

private:
    std::function<void(const std::shared_ptr<NetworkResponse>& response)> transformCallback(UNIVERSAL_RESPONSE_CALLBACK callback)
    {
        auto callbackWrapper = [callback](const std::shared_ptr<NetworkResponse>& response) {
            std::shared_ptr<ResponseError> error = std::make_shared<ResponseError>(response->code,
                                                                                   response->errorType,
                                                                                   response->errorDescription);
           if (callback) {
                callback(response->data, response->header, error);
            }
        };
        return callbackWrapper;
    }

private:
    std::string _host;
    int16_t _port = -1;
    std::string _path;
    std::string _query;
    NetworkMethod _method;
    std::vector<uint8_t> _data;
    std::string _contentType;// = ContentType::json();
    std::unordered_map<std::string, std::string> _header;
    int64_t _retryCount = 0;
    int64_t _timeout;
    RequestPriority _priority;
    RESPONSE_CALLBACK _callback;
    std::string _pluginName;
    RequestRoute _requestRoute;
};

}
