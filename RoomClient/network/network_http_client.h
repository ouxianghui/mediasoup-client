#pragma once

#include <memory>
#include "network.hpp"

namespace vi {

class NetworkHttpClient : public INetworkClient, public std::enable_shared_from_this<NetworkHttpClient>
{
public:
    NetworkHttpClient();

    ~NetworkHttpClient() override;

    void request(const std::shared_ptr<NetworkRequest>& request, const std::shared_ptr<INetworkCallback>& callback) override;

    void init() override;

    void reset() override;

    bool isNetworkReachable() override;

    bool isAvailable() override;

    void cancelRequest(const std::shared_ptr<NetworkRequest>& request) override;

private:
    void handleResults(int64_t requestID, std::shared_ptr<NetworkResponse> response, std::shared_ptr<INetworkCallback> callback);

private:
};

}
