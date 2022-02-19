#pragma once

#include <memory>
#include "i_participant_controller.h"
#include "i_media_controller_observer.h"
#include "utils/universal_observable.hpp"
#include "i_signaling_observer.h"

namespace vi {

class ParticipantController :
        public IParticipantController,
        public ISignalingObserver,
        public UniversalObservable<IParticipantControllerObserver>,
        public std::enable_shared_from_this<ParticipantController>
{
public:
    ParticipantController();

    ~ParticipantController();

    void init() override;

    void destroy() override;

    void addObserver(std::shared_ptr<IParticipantControllerObserver> observer) override;

    void removeObserver(std::shared_ptr<IParticipantControllerObserver> observer) override;

    std::shared_ptr<IParticipant> getParticipant(const std::string& pid) override;

    std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<IParticipant>>> getParticipants() override;

protected:
    // ISignalingObserver
    void onOpened() override;

    void onClosed() override;

    // Request from SFU
    void onNewConsumer(std::shared_ptr<signaling::NewConsumerRequest> request) override;

    void onNewDataConsumer(std::shared_ptr<signaling::NewDataConsumerRequest> request) override;

    // Notification from SFU
    void onProducerScore(std::shared_ptr<signaling::ProducerScoreNotification> notification) override;

    void onConsumerScore(std::shared_ptr<signaling::ConsumerScoreNotification> notification) override;

    void onNewPeer(std::shared_ptr<signaling::NewPeerNotification> notification) override;

    void onPeerClosed(std::shared_ptr<signaling::PeerClosedNotification> notification) override;

    void onPeerDisplayNameChanged(std::shared_ptr<signaling::PeerDisplayNameChangedNotification> notification) override;

    void onConsumerPaused(std::shared_ptr<signaling::ConsumerPausedNotification> notification) override;

    void onConsumerResumed(std::shared_ptr<signaling::ConsumerResumedNotification> notification) override;

    void onConsumerClosed(std::shared_ptr<signaling::ConsumerClosedNotification> notification) override;

    void onConsumerLayersChanged(std::shared_ptr<signaling::ConsumerLayersChangedNotification> notification) override;

    void onDataConsumerClosed(std::shared_ptr<signaling::DataConsumerClosedNotification> notification) override;

    void onDownlinkBwe(std::shared_ptr<signaling::DownlinkBweNotification> notification) override;

    void onActiveSpeaker(std::shared_ptr<signaling::ActiveSpeakerNotification> notification) override;

private:
    // key: peer id
    std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<IParticipant>>> _participantMap;
};

}
