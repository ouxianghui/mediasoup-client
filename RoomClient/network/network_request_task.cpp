#include "network_request_task.h"

const int32_t kRequestWeightNormal = 10; 
const int32_t kRequestWeightHigh = 20;

namespace vi {

NetworkRequestTask::NetworkRequestTask(const std::shared_ptr<NetworkRequest>& request)
    : _request(request)
{
    setPriority(_request->priority());
}

NetworkRequestTask::~NetworkRequestTask()
{

}

void NetworkRequestTask::setPriority(RequestPriority priority)
{
    if (_request) {
        _request->setPriority(priority);
    }

    switch (priority)
    {
    case vi::RequestPriority::LOW:
        _weight = 0;
        break;
    case vi::RequestPriority::NORMAL:
        if (_weight < kRequestWeightNormal) {
            _weight = kRequestWeightNormal;
        }
        break;
    case vi::RequestPriority::HIGH:
        if (_weight < kRequestWeightHigh) {
            _weight = kRequestWeightHigh;
        }
        break;
    case vi::RequestPriority::SEPECIFIC:
        if (_weight < kRequestWeightHigh) {
            _weight = kRequestWeightHigh;
        }
        break;
    default:
        break;
    }
}

RequestPriority NetworkRequestTask::priority() const
{
    return _request->priority();
}

const std::shared_ptr<NetworkRequest>& NetworkRequestTask::request()
{
    return _request;
}

RequestRoute NetworkRequestTask::requestRoute() const
{
    return _request->requestRoute();
}

void NetworkRequestTask::setWeight(int32_t weight)
{
    _weight = weight;
}

int32_t NetworkRequestTask::weight() const
{
    return _weight;
}

}
