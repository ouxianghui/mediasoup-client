#pragma once

#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include "network.hpp"

namespace vi {

class NetworkRequestTask;

class NetworkRequestPlugin :
        public INetworkRequestHandler,
        public INetworkStatusListener,
        public INetworkRequestProducer,
        public std::enable_shared_from_this<NetworkRequestPlugin>
{
public:
    NetworkRequestPlugin();

    ~NetworkRequestPlugin() override;

    void addRequestConsumer(RequestRoute route, const std::shared_ptr<INetworkRequestConsumer>& consumer);

    // INetworkRequestHandler
    void addRequest(const std::shared_ptr<NetworkRequest>& request, bool isTail) override;

    void cancelAll() override;

    void cancelRequest(const std::shared_ptr<NetworkRequest>& request) override;

    std::string pluginName() const override;

    // INetworkStatusListener
    void onNetworkStatusChanged(bool online) override;

    // INetworkRequestProducer
    const std::shared_ptr<NetworkRequest> produceRequest(RequestRoute route) override;

private:
    void notifyRequestArrived(RequestRoute route);

    void appendTask(const std::shared_ptr<NetworkRequestTask>& task, bool isTail = true);

    void cancelAllConsumers();

    void cancelAllPendingTasks();

    bool isRequestInPending(const std::shared_ptr<NetworkRequest>& request);

    bool canProduceRequest(RequestPriority priority);

    bool isConsumerAvailable(RequestRoute route);

    bool canBeHandled(RequestRoute route, const std::shared_ptr<NetworkRequestTask>& task);

    std::shared_ptr<NetworkRequestTask> nextTaskInQueue(RequestRoute route, std::deque<std::shared_ptr<NetworkRequestTask>>& queue);

    void changeTaskWeight(std::deque<std::shared_ptr<NetworkRequestTask>>& source, std::deque<std::shared_ptr<NetworkRequestTask>>& target, int32_t weight);

    void responseCallback(NetworkErrorType type, const std::shared_ptr<NetworkRequest>& request);

private:
    std::string _pluginName;

    std::unordered_map<RequestRoute, std::shared_ptr<INetworkRequestConsumer>> _consumers;

    std::unordered_map<RequestPriority, std::deque<std::shared_ptr<NetworkRequestTask>>> _pendingTasks;

    std::mutex _lock;
};

}
