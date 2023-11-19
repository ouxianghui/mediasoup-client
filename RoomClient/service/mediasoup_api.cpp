/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#include "mediasoup_api.h"
#include <string>
#include "logger/spd_logger.h"

namespace vi {

    MediasoupApi::MediasoupApi(std::weak_ptr<ISignalingClient> wsc)
    : _wsc(wsc)
    {

    }

    MediasoupApi::~MediasoupApi()
    {
        DLOG("~MediasoupApi()");
    }

    void MediasoupApi::init()
    {

    }

    void MediasoupApi::destroy()
    {

    }

    void MediasoupApi::getRouterRtpCapabilities(std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetRouterRtpCapabilitiesResponse>)> callback)
    {
        send<signaling::GetRouterRtpCapabilitiesRequest, signaling::GetRouterRtpCapabilitiesResponse>(std::make_shared<signaling::GetRouterRtpCapabilitiesRequest>(), callback);
    }

    void MediasoupApi::createWebRtcTransport(std::shared_ptr<signaling::CreateWebRtcTransportRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::CreateWebRtcTransportResponse>)> callback)
    {
        send<signaling::CreateWebRtcTransportRequest, signaling::CreateWebRtcTransportResponse>(request, callback);
    }

    void MediasoupApi::join(std::shared_ptr<signaling::JoinRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::JoinResponse>)> callback)
    {
        send<signaling::JoinRequest, signaling::JoinResponse>(request, callback);
    }

    void MediasoupApi::connectWebRtcTransport(std::shared_ptr<signaling::ConnectWebRtcTransportRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        send<signaling::ConnectWebRtcTransportRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::produce(std::shared_ptr<signaling::ProduceRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::ProduceResponse>)> callback)
    {
        send<signaling::ProduceRequest, signaling::ProduceResponse>(request, callback);
    }

    void MediasoupApi::produceData(std::shared_ptr<signaling::ProduceDataRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::ProduceDataResponse>)> callback)
    {
        send<signaling::ProduceDataRequest, signaling::ProduceDataResponse>(request, callback);
    }

    void MediasoupApi::pauseProducer(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        auto request = std::make_shared<signaling::ProducerRequest>();
        request->method = "pauseProducer";
        request->data = signaling::ProducerRequest::Data();
        request->data->producerId = producerId;
        send<signaling::ProducerRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::resumeProducer(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        auto request = std::make_shared<signaling::ProducerRequest>();
        request->method = "resumeProducer";
        request->data = signaling::ProducerRequest::Data();
        request->data->producerId = producerId;
        send<signaling::ProducerRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::closeProducer(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        auto request = std::make_shared<signaling::ProducerRequest>();
        request->method = "closeProducer";
        request->data = signaling::ProducerRequest::Data();
        request->data->producerId = producerId;
        send<signaling::ProducerRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::pauseConsumer(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        auto request = std::make_shared<signaling::ConsumerRequest>();
        request->method = "pauseConsumer";
        request->data = signaling::ConsumerRequest::Data();
        request->data->consumerId = consumerId;
        send<signaling::ConsumerRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::resumeConsumer(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        auto request = std::make_shared<signaling::ConsumerRequest>();
        request->method = "resumeConsumer";
        request->data = signaling::ConsumerRequest::Data();
        request->data->consumerId = consumerId;
        send<signaling::ConsumerRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::changeDisplayName(const std::string& displayName, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        auto request = std::make_shared<signaling::ChangeDisplayNameRequest>();
        request->data = signaling::ChangeDisplayNameRequest::Data();
        request->data->displayName = displayName;
        send<signaling::ChangeDisplayNameRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::restartICE(const std::string& transportId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::RestartICEResponse>)> callback)
    {
        auto request = std::make_shared<signaling::RestartICERequest>();
        request->data = signaling::RestartICERequest::Data();
        request->data->transportId = transportId;
        send<signaling::RestartICERequest, signaling::RestartICEResponse>(request, callback);
    }

    void MediasoupApi::requestConsumerKeyFrame(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        auto request = std::make_shared<signaling::ConsumerRequest>();
        request->method = "requestConsumerKeyFrame";
        request->data = signaling::ConsumerRequest::Data();
        request->data->consumerId = consumerId;
        send<signaling::ConsumerRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::setConsumerPriority(const std::string& consumerId, int32_t priority, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        auto request = std::make_shared<signaling::SetConsumerPriorityRequest>();
        request->data = signaling::SetConsumerPriorityRequest::Data();
        request->data->consumerId = consumerId;
        request->data->priority = priority;
        send<signaling::SetConsumerPriorityRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::setConsumerPreferredLayers(const std::string& consumerId, int32_t spatialLayer, int32_t temporalLayer, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        auto request = std::make_shared<signaling::SetConsumerPreferredLayersRequest>();
        request->data = signaling::SetConsumerPreferredLayersRequest::Data();
        request->data->consumerId = consumerId;
        request->data->spatialLayer = spatialLayer;
        request->data->temporalLayer = temporalLayer;
        send<signaling::SetConsumerPreferredLayersRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::getTransportStats(const std::string& transportId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetTransportStatsResponse>)> callback)
    {
        auto request = std::make_shared<signaling::GetTransportStatsRequest>();
        request->data = signaling::GetTransportStatsRequest::Data();
        request->data->transportId = transportId;
        send<signaling::GetTransportStatsRequest, signaling::GetTransportStatsResponse>(request, callback);
    }

    void MediasoupApi::getProducerStats(const std::string& producerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetProducerStatsResponse>)> callback)
    {
        auto request = std::make_shared<signaling::GetProducerStatsRequest>();
        request->data = signaling::GetProducerStatsRequest::Data();
        request->data->producerId = producerId;
        send<signaling::GetProducerStatsRequest, signaling::GetProducerStatsResponse>(request, callback);
    }

    void MediasoupApi::getDataProducerStats(const std::string& dataProducerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetDataProducerStatsResponse>)> callback)
    {
        auto request = std::make_shared<signaling::GetDataProducerStatsRequest>();
        request->data = signaling::GetDataProducerStatsRequest::Data();
        request->data->dataProducerId = dataProducerId;
        send<signaling::GetDataProducerStatsRequest, signaling::GetDataProducerStatsResponse>(request, callback);
    }

    void MediasoupApi::getConsumerStats(const std::string& consumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetConsumerStatsResponse>)> callback)
    {
        auto request = std::make_shared<signaling::GetConsumerStatsRequest>();
        request->data = signaling::GetConsumerStatsRequest::Data();
        request->data->consumerId = consumerId;
        send<signaling::GetConsumerStatsRequest, signaling::GetConsumerStatsResponse>(request, callback);
    }

    void MediasoupApi::getDataConsumerStats(const std::string& dataConsumerId, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::GetDataConsumerStatsResponse>)> callback)
    {
        auto request = std::make_shared<signaling::GetDataConsumerStatsRequest>();
        request->data = signaling::GetDataConsumerStatsRequest::Data();
        request->data->dataConsumerId = dataConsumerId;
        send<signaling::GetDataConsumerStatsRequest, signaling::GetDataConsumerStatsResponse>(request, callback);
    }

    void MediasoupApi::applyNetworkThrottle(std::shared_ptr<signaling::ApplyNetworkThrottleRequest> request, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        send<signaling::ApplyNetworkThrottleRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::resetNetworkThrottle(const std::string& secret, std::function<void(int32_t, const std::string&, std::shared_ptr<signaling::BasicResponse>)> callback)
    {
        auto request = std::make_shared<signaling::ResetNetworkThrottleRequest>();
        request->data = signaling::ResetNetworkThrottleRequest::Data();
        request->data->secret = secret;
        send<signaling::ResetNetworkThrottleRequest, signaling::BasicResponse>(request, callback);
    }

    void MediasoupApi::response(std::shared_ptr<signaling::BasicResponse> response)
    {
        if (!response) {
            DLOG("request is null");
            return;
        }

        auto sc = _wsc.lock();
        if (!sc) {
            DLOG("sc is null");
            return;
        }

        sc->send(response->toJsonStr(), response->id.value_or(-1), nullptr, nullptr);
    }

}
