#pragma once

#include <memory>
#include <mutex>
#include "network.hpp"

namespace vi {

class NetworkRequestConsumer :
        public INetworkRequestConsumer,
        public INetworkRequestExecutorListener,
        public std::enable_shared_from_this<NetworkRequestConsumer>
{
public:
    NetworkRequestConsumer(const std::shared_ptr<INetworkRequestProducer>& producer,
                           const std::shared_ptr<INetworkClient>& client,
                           int32_t maxQueueCount,
                           RequestRoute route);
    ~NetworkRequestConsumer() override;

    // INetworkRequestConsumer
    void onNetworkRequestArrived() override;

    bool canHandleRequest(RequestRoute route) override;

    bool isAvailiable() override;

    void cancelAll() override;

    void cancelRequest(const std::shared_ptr<NetworkRequest>& request) override;

    // INetworkRequestExecutorListener
    void onComplete(const std::shared_ptr<NetworkRequestExecutor>& executor) override;

private:
    void execute();

    bool canHandleRequest();

    void addRequestExecutor(const std::shared_ptr<NetworkRequestExecutor>& executor);

    void removeRequestExecutor(const std::shared_ptr<NetworkRequestExecutor>& executor);

    std::shared_ptr<NetworkRequestExecutor> requestExecutor(int64_t id);

private:
    std::shared_ptr<INetworkRequestProducer> _producer;

    std::shared_ptr<INetworkClient> _client;

    std::unordered_map<int64_t, std::shared_ptr<NetworkRequestExecutor>> _requestQueue;

    int32_t _maxQueueCount;

    RequestRoute _requestRoute;

    std::mutex _lock;
};

}
