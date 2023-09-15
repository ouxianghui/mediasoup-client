#include "network_request_consumer.h"
#include "network_request_executor.h"
//#include <QDebug>
#include "logger/spd_logger.h"

namespace vi {

NetworkRequestConsumer::NetworkRequestConsumer(const std::shared_ptr<INetworkRequestProducer>& producer,
                                               const std::shared_ptr<INetworkClient>& client,
                                               int32_t maxQueueCount,
                                               RequestRoute route)
    : _producer(producer)
    , _client(client)
    , _requestQueue()
    , _maxQueueCount(maxQueueCount)
    , _requestRoute(route)
{

}

NetworkRequestConsumer::~NetworkRequestConsumer()
{

}

// INetworkRequestConsumer
void NetworkRequestConsumer::onNetworkRequestArrived()
{
    execute();
}

bool NetworkRequestConsumer::canHandleRequest(RequestRoute route)
{
    return route == _requestRoute;
}

bool NetworkRequestConsumer::isAvailiable()
{
    return _client->isAvailable();
}

void NetworkRequestConsumer::cancelAll()
{
    {
        std::lock_guard<std::mutex> guard(_lock);
        for (auto iter = _requestQueue.begin(); iter != _requestQueue.end(); ++iter) {
            (iter->second)->cancel();
        }
    }
}

void NetworkRequestConsumer::cancelRequest(const std::shared_ptr<NetworkRequest>& request)
{
    std::shared_ptr<NetworkRequestExecutor> executor = requestExecutor(request->requestId());

    if (executor != nullptr)
    {
        executor->cancel();
    }
}

// IRequestExecutorListener
void NetworkRequestConsumer::onComplete(const std::shared_ptr<NetworkRequestExecutor>& executor)
{
    DLOG("onComplete()");

    removeRequestExecutor(executor);

    execute();
}

// private
void NetworkRequestConsumer::execute()
{
    DLOG("execute()");

    if (!canHandleRequest()) {
        WLOG(" request queue is full");
        return;
    }

    auto request = _producer->produceRequest(_requestRoute);

    if (request == nullptr) {
        DLOG("idel");
        return;
    }

    request->setRequestRoute(_requestRoute);

    std::shared_ptr<NetworkRequestExecutor> executor = std::make_shared<NetworkRequestExecutor>(request, _requestRoute, _client);

    executor->setListener(shared_from_this());

    addRequestExecutor(executor);

    executor->execute();
}

bool NetworkRequestConsumer::canHandleRequest()
{
    std::lock_guard<std::mutex> guard(_lock);

    return (int32_t)_requestQueue.size() < _maxQueueCount;
}

void NetworkRequestConsumer::addRequestExecutor(const std::shared_ptr<NetworkRequestExecutor>& executor)
{
    {
        std::lock_guard<std::mutex> guard(_lock);

        _requestQueue[executor->requestId()] = executor;
    }
}

void NetworkRequestConsumer::removeRequestExecutor(const std::shared_ptr<NetworkRequestExecutor>& executor)
{
    {
        std::lock_guard<std::mutex> guard(_lock);

        auto iter = _requestQueue.find(executor->requestId());
        if (iter != _requestQueue.end()) {
            _requestQueue.erase(iter);
        }
    }
}

std::shared_ptr<NetworkRequestExecutor> NetworkRequestConsumer::requestExecutor(int64_t id)
{
    std::shared_ptr<NetworkRequestExecutor> executor = nullptr;

    {
        std::lock_guard<std::mutex> guard(_lock);

        auto iter = _requestQueue.find(id);

        if (iter != _requestQueue.end()) {
            executor = iter->second;
        }
    }

    return executor;
}

}
