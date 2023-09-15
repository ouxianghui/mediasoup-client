#pragma once

#include <memory>
#include "network.hpp"

extern const int32_t kRequestWeightNormal;
extern const int32_t kRequestWeightHigh;

namespace vi {

class NetworkRequestTask
{
public:
    NetworkRequestTask(const std::shared_ptr<NetworkRequest>& request);

    virtual ~NetworkRequestTask();

    const std::shared_ptr<NetworkRequest>& request();

    void setPriority(RequestPriority priority);

    RequestPriority priority() const;

    RequestRoute requestRoute() const;

    void setWeight(int32_t weight);

    int32_t weight() const;

private:
    std::shared_ptr<NetworkRequest> _request;

    int32_t _weight;
};

}
