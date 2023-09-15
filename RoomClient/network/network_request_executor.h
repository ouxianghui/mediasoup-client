#pragma once

#include <memory>
#include "network.hpp"

namespace vi {

class NetworkRequestExecutor :
        public INetworkCallback,
        public std::enable_shared_from_this<NetworkRequestExecutor>
{
public:
    NetworkRequestExecutor(const std::shared_ptr<NetworkRequest>& request,
                           RequestRoute route,
                           const std::shared_ptr<INetworkClient>& client);

    ~NetworkRequestExecutor() override;

    // INetworkCallback
    void onSuccess(int64_t requestID, const std::shared_ptr<NetworkResponse>& response) override;

    void onFailure(int64_t requestID, const std::shared_ptr<NetworkResponse>& response) override;

    void onProgress(int64_t requestID, int32_t max_size, int32_t currentPos) override;

    int64_t requestId() const;

    void execute();

    void cancel();

    void setListener(const std::weak_ptr<INetworkRequestExecutorListener>& listener);

private:
    void notifyCompletion();

    void responseCallback(int64_t code, NetworkErrorType errorType);

    void responseCallback(const std::shared_ptr<NetworkResponse>& response);

private:
    std::shared_ptr<NetworkRequest> _request;

    RequestRoute _requestRoute;

    std::shared_ptr<INetworkClient> _client;

    std::weak_ptr<INetworkRequestExecutorListener> _listener;

    std::string _pluginName;
};

}
