#include "participant_controller_observer_wrapper.h"
#include "api/media_stream_interface.h"

ParticipantControllerObserverWrapper::ParticipantControllerObserverWrapper(QObject* parent) : QObject(parent)
{

}

void ParticipantControllerObserverWrapper::init()
{

}

void ParticipantControllerObserverWrapper::destroy()
{

}

void ParticipantControllerObserverWrapper::onParticipantJoin(std::shared_ptr<vi::IParticipant> participant)
{
    emit participantJoin(participant);
}

void ParticipantControllerObserverWrapper::onParticipantLeave(std::shared_ptr<vi::IParticipant> participant)
{
    emit participantLeave(participant);
}

void ParticipantControllerObserverWrapper::onRemoteActiveSpeaker(std::shared_ptr<vi::IParticipant> participant, int32_t volume)
{
    emit remoteActiveSpeaker(participant, volume);
}

void ParticipantControllerObserverWrapper::onDisplayNameChanged(std::shared_ptr<vi::IParticipant> participant)
{
    emit displayNameChanged(participant);
}

void ParticipantControllerObserverWrapper::onCreateRemoteVideoTrack(std::shared_ptr<vi::IParticipant> participant, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    emit createRemoteVideoTrack(participant, tid, track);
}

void ParticipantControllerObserverWrapper::onRemoveRemoteVideoTrack(std::shared_ptr<vi::IParticipant> participant, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    emit removeRemoteVideoTrack(participant, tid, track);
}

void ParticipantControllerObserverWrapper::onRemoteAudioStateChanged(std::shared_ptr<vi::IParticipant> participant, bool muted)
{
    emit remoteAudioStateChanged(participant, muted);
}

void ParticipantControllerObserverWrapper::onRemoteVideoStateChanged(std::shared_ptr<vi::IParticipant> participant, bool muted)
{
    emit remoteVideoStateChanged(participant, muted);
}
