/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>
#include "i_signaling_client.h"
#include "websocket/i_transport_observer.h"
#include "utils/universal_observable.hpp"
#include "i_signaling_event_handler.h"

namespace rtc {
    class Thread;
}

namespace vi {

class WebsocketRequest;
class ITransport;

class SignalingClient : public ISignalingClient, public ITransportObserver, public UniversalObservable<ISignalingEventHandler>, public std::enable_shared_from_this<SignalingClient>
{
public:
    SignalingClient(rtc::Thread* thread);

    ~SignalingClient();

    void init() override;

    void destroy() override;

    void addObserver(std::shared_ptr<ISignalingEventHandler> observer) override;

    void removeObserver(std::shared_ptr<ISignalingEventHandler> observer) override;

    void connect(const std::string& url, const std::string& subprotocol) override;

    void disconnect() override;

    void send(const std::string& text, int64_t transcation, SuccessCallback scb, FailureCallback fcb) override;

    void send(const std::vector<uint8_t>& data, int64_t transcation, SuccessCallback scb, FailureCallback fcb) override;

protected:
    void onOpened() override;

    void onClosed() override;

    void onFailed(int errorCode, const std::string& reason) override;

    void onMessage(const std::string& json) override;

private:
    void handleRequest(const std::string& json);

    void handleResponse(const std::string& json);

    void handleNotification(const std::string& json);

    void clearRequests();

private:
    rtc::Thread* _thread;

    std::shared_ptr<ITransport> _transport;

    std::mutex _requestMutex;

    std::unordered_map<int64_t, std::shared_ptr<WebsocketRequest>> _requestMap;
};

}


