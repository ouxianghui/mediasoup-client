#pragma once

#include <memory>
#include <functional>
#include "signaling_models.h"

namespace vi {

class IMediasoupApi {
public:
    virtual ~IMediasoupApi() = default;

    virtual void init() = 0;

    virtual void destroy() = 0;

    virtual void getRouterRtpCapabilities(std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetRouterRtpCapabilitiesResponse>)> callback) = 0;

    virtual void createWebRtcTransport(std::shared_ptr<signaling::CreateWebRtcTransportRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::CreateWebRtcTransportResponse>)> callback) = 0;

    virtual void join(std::shared_ptr<signaling::JoinRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void connectWebRtcTransport(std::shared_ptr<signaling::ConnectWebRtcTransportRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void produce(std::shared_ptr<signaling::ProduceRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::ProduceResponse>)> callback) = 0;

    virtual void produceData(std::shared_ptr<signaling::ProduceDataRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::ProduceDataResponse>)> callback) = 0;

    virtual void pauseProducer(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void resumeProducer(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void closeProducer(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void pauseConsumer(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void resumeConsumer(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void changeDisplayName(const std::string& displayName, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void restartICE(const std::string& transportId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::RestartICEResponse>)> callback) = 0;

    virtual void requestConsumerKeyFrame(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void setConsumerPriority(const std::string& consumerId, int32_t priority, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void setConsumerPreferredLayers(const std::string& consumerId, int32_t spatialLayer, int32_t temporalLayer, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void getTransportStats(const std::string& transportId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetTransportStatsResponse>)> callback) = 0;

    virtual void getProducerStats(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetProducerStatsResponse>)> callback) = 0;

    virtual void getDataProducerStats(const std::string& dataProducerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetDataProducerStatsResponse>)> callback) = 0;

    virtual void getConsumerStats(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetConsumerStatsResponse>)> callback) = 0;

    virtual void getDataConsumerStats(const std::string& dataConsumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetDataConsumerStatsResponse>)> callback) = 0;

    virtual void applyNetworkThrottle(std::shared_ptr<signaling::ApplyNetworkThrottleRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void resetNetworkThrottle(const std::string& secret, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback) = 0;

    virtual void response(std::shared_ptr<signaling::BasicResponse> response) = 0;
};

}
