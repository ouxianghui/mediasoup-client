#pragma once

#include <string>
#include <memory>
#include "network.hpp"

namespace vi {

class INetworkRequestManager {
public:
    virtual ~INetworkRequestManager() = default;

    virtual void init() = 0;

    virtual void registerPlugin(const std::string& name, std::shared_ptr<INetworkRequestHandler> plugin) = 0;

    virtual void unregisterPlugin(const std::string& name) = 0;

    virtual void addRequest(const std::shared_ptr<NetworkRequest>& request, bool isTail = true) = 0;

    virtual void cancelAll() = 0;

    virtual void cancelAll(const std::string& pluginName) = 0;

    virtual void cancelRequest(const std::shared_ptr<NetworkRequest>& request) = 0;
};

}
