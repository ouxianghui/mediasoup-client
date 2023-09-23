#pragma once

#include <memory>
#include "signaling_models.h"

namespace vi {

class ISignalingEventHandler {
public:
    virtual ~ISignalingEventHandler() = default;

    virtual void onOpened() = 0;

    virtual void onClosed() = 0;

    // Request from SFU
    virtual void onNewConsumer(std::shared_ptr<signaling::NewConsumerRequest> request) = 0;

    virtual void onNewDataConsumer(std::shared_ptr<signaling::NewDataConsumerRequest> request) = 0;

    // Notification from SFU
    virtual void onProducerScore(std::shared_ptr<signaling::ProducerScoreNotification> notification) = 0;

    virtual void onConsumerScore(std::shared_ptr<signaling::ConsumerScoreNotification> notification) = 0;

    virtual void onNewPeer(std::shared_ptr<signaling::NewPeerNotification> notification) = 0;

    virtual void onPeerClosed(std::shared_ptr<signaling::PeerClosedNotification> notification) = 0;

    virtual void onPeerDisplayNameChanged(std::shared_ptr<signaling::PeerDisplayNameChangedNotification> notification) = 0;

    virtual void onConsumerPaused(std::shared_ptr<signaling::ConsumerPausedNotification> notification) = 0;

    virtual void onConsumerResumed(std::shared_ptr<signaling::ConsumerResumedNotification> notification) = 0;

    virtual void onConsumerClosed(std::shared_ptr<signaling::ConsumerClosedNotification> notification) = 0;

    virtual void onConsumerLayersChanged(std::shared_ptr<signaling::ConsumerLayersChangedNotification> notification) = 0;

    virtual void onDataConsumerClosed(std::shared_ptr<signaling::DataConsumerClosedNotification> notification) = 0;

    virtual void onDownlinkBwe(std::shared_ptr<signaling::DownlinkBweNotification> notification) = 0;

    virtual void onActiveSpeaker(std::shared_ptr<signaling::ActiveSpeakerNotification> notification) = 0;

};

}
