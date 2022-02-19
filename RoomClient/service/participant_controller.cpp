#include "participant_controller.h"

namespace vi {

ParticipantController::ParticipantController()
{

}

ParticipantController::~ParticipantController()
{

}

void ParticipantController::init()
{
    _participantMap = std::make_shared<std::unordered_map<std::string, std::shared_ptr<IParticipant>>>();
}

void ParticipantController::destroy()
{
    if (_participantMap) {
        _participantMap->clear();
        _participantMap = nullptr;
    }
}

void ParticipantController::addObserver(std::shared_ptr<IParticipantControllerObserver> observer)
{
    UniversalObservable<IParticipantControllerObserver>::addWeakObserver(observer, "mediasoup-client");
}

void ParticipantController::removeObserver(std::shared_ptr<IParticipantControllerObserver> observer)
{
    UniversalObservable<IParticipantControllerObserver>::removeObserver(observer);
}

std::shared_ptr<IParticipant> ParticipantController::getParticipant(const std::string& pid)
{
    if (!_participantMap) {
        return nullptr;
    }

    if (_participantMap->find(pid) != _participantMap->end()) {
        return (*_participantMap)[pid];
    }

    return nullptr;
}

std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<IParticipant>>> ParticipantController::getParticipants()
{
    return _participantMap;
}

void ParticipantController::onOpened()
{

}

void ParticipantController::onClosed()
{

}

// Request from SFU
void ParticipantController::onNewConsumer(std::shared_ptr<signaling::NewConsumerRequest> request)
{

}

void ParticipantController::onNewDataConsumer(std::shared_ptr<signaling::NewDataConsumerRequest> request)
{

}

// Notification from SFU
void ParticipantController::onProducerScore(std::shared_ptr<signaling::ProducerScoreNotification> notification)
{

}

void ParticipantController::onConsumerScore(std::shared_ptr<signaling::ConsumerScoreNotification> notification)
{

}

void ParticipantController::onNewPeer(std::shared_ptr<signaling::NewPeerNotification> notification)
{

}

void ParticipantController::onPeerClosed(std::shared_ptr<signaling::PeerClosedNotification> notification)
{

}

void ParticipantController::onPeerDisplayNameChanged(std::shared_ptr<signaling::PeerDisplayNameChangedNotification> notification)
{

}

void ParticipantController::onConsumerPaused(std::shared_ptr<signaling::ConsumerPausedNotification> notification)
{

}

void ParticipantController::onConsumerResumed(std::shared_ptr<signaling::ConsumerResumedNotification> notification)
{

}

void ParticipantController::onConsumerClosed(std::shared_ptr<signaling::ConsumerClosedNotification> notification)
{

}

void ParticipantController::onConsumerLayersChanged(std::shared_ptr<signaling::ConsumerLayersChangedNotification> notification)
{

}

void ParticipantController::onDataConsumerClosed(std::shared_ptr<signaling::DataConsumerClosedNotification> notification)
{

}

void ParticipantController::onDownlinkBwe(std::shared_ptr<signaling::DownlinkBweNotification> notification)
{

}

void ParticipantController::onActiveSpeaker(std::shared_ptr<signaling::ActiveSpeakerNotification> notification)
{

}

}
