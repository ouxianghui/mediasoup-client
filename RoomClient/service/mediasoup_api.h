#pragma once

#include <memory>
#include "i_mediasoup_api.h"
#include "component_factory.h"
#include "signaling_client.h"
#include "logger/u_logger.h"

namespace vi {

class MediasoupApi : public IMediasoupApi, public std::enable_shared_from_this<MediasoupApi>
{
public:
    MediasoupApi(std::weak_ptr<ISignalingClient> wsc);

    ~MediasoupApi();

    void init() override;

    void destroy() override;

    void getRouterRtpCapabilities(std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetRouterRtpCapabilitiesResponse>)> callback) override;

    void createWebRtcTransport(std::shared_ptr<signaling::CreateWebRtcTransportRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::CreateWebRtcTransportResponse>)> callback) override;

    void join(std::shared_ptr<signaling::JoinRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void connectWebRtcTransport(std::shared_ptr<signaling::ConnectWebRtcTransportRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void produce(std::shared_ptr<signaling::ProduceRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::ProduceResponse>)> callback) override;

    void produceData(std::shared_ptr<signaling::ProduceDataRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::ProduceDataResponse>)> callback) override;

    void pauseProducer(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void resumeProducer(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void closeProducer(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void pauseConsumer(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void resumeConsumer(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void changeDisplayName(const std::string& displayName, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void restartICE(const std::string& transportId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::RestartICEResponse>)> callback) override;

    void requestConsumerKeyFrame(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void setConsumerPriority(const std::string& consumerId, int32_t priority, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void setConsumerPreferredLayers(const std::string& consumerId, int32_t spatialLayer, int32_t temporalLayer, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void getTransportStats(const std::string& transportId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetTransportStatsResponse>)> callback) override;

    void getProducerStats(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetProducerStatsResponse>)> callback) override;

    void getDataProducerStats(const std::string& dataProducerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetDataProducerStatsResponse>)> callback) override;

    void getConsumerStats(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetConsumerStatsResponse>)> callback) override;

    void getDataConsumerStats(const std::string& dataConsumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetDataConsumerStatsResponse>)> callback) override;

    void applyNetworkThrottle(std::shared_ptr<signaling::ApplyNetworkThrottleRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void resetNetworkThrottle(const std::string& secret, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) override;

    void response(std::shared_ptr<signaling::BasicResponse> response) override;

private:
    template<typename Request, typename Response>
    void send(std::shared_ptr<Request> request, std::function<void(int32_t, const std::string&, std::shared_ptr<Response>)> callback)
    {
        if (!request) {
            DLOG("request is null");
            return;
        }

        auto sc = _wsc.lock();
        if (!sc) {
             DLOG("sc is null");
            return;
        }

        sc->send(request->toJsonStr(), request->id.value_or(-1), [callback](const std::string& json){
            if (json.empty()) {
                return;
            }
            std::string err;
            auto response = fromJsonString<Response>(json, err);
            if (!err.empty()) {
                DLOG("parse response failed: {}", err);
                return;
            }
            if (callback) {
                callback(0, "", response);
            }
        }, [callback](int32_t errorCode, const std::string& errorInfo){
            if (callback) {
                callback(errorCode, errorInfo, nullptr);
            }
        });
    }

private:
    std::weak_ptr<ISignalingClient> _wsc;
};

}
