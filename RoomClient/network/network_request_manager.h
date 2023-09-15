#pragma once

#include "i_network_request_manager.h"
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <chrono>
#include <mutex>
#include "network.hpp"

namespace vi {

class NetworkRequestManager : public INetworkRequestManager, public std::enable_shared_from_this<NetworkRequestManager> {
public:
    NetworkRequestManager();

    ~NetworkRequestManager();

    void init() override;

	void registerPlugin(const std::string& name, std::shared_ptr<INetworkRequestHandler> plugin) override;

	void unregisterPlugin(const std::string& name) override;

    void addRequest(const std::shared_ptr<NetworkRequest>& request, bool isTail = true) override;

    void cancelAll() override;

    void cancelAll(const std::string& pluginName) override;

    void cancelRequest(const std::shared_ptr<NetworkRequest>& request) override;

private:
    std::mutex _lock;

    std::unordered_map<std::string, std::shared_ptr<INetworkRequestHandler>> _plugins;
};

}
