#include "participant_event_adapter.h"

ParticipantEventAdapter::ParticipantEventAdapter(QObject *parent)
    : QObject{parent}
{

}

void ParticipantEventAdapter::onCreateParticipant(std::shared_ptr<vi::IParticipant> participant)
{
    emit participantCreated(participant);
}

void ParticipantEventAdapter::onUpdateParticipant(std::shared_ptr<vi::IParticipant> participant)
{
    emit participantUpdated(participant);
}

void ParticipantEventAdapter::onRemoveParticipant(std::shared_ptr<vi::IParticipant> participant)
{
    emit participantRemoved(participant);
}

