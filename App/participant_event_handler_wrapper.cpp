#include "participant_event_handler_wrapper.h"
#include "api/media_stream_interface.h"

ParticipantEventHandlerWrapper::ParticipantEventHandlerWrapper(QObject* parent) : QObject(parent)
{

}

void ParticipantEventHandlerWrapper::init()
{

}

void ParticipantEventHandlerWrapper::destroy()
{

}

void ParticipantEventHandlerWrapper::onParticipantJoin(std::shared_ptr<vi::IParticipant> participant)
{
    emit participantJoin(participant);
}

void ParticipantEventHandlerWrapper::onParticipantLeave(std::shared_ptr<vi::IParticipant> participant)
{
    emit participantLeave(participant);
}

void ParticipantEventHandlerWrapper::onRemoteActiveSpeaker(std::shared_ptr<vi::IParticipant> participant, int32_t volume)
{
    emit remoteActiveSpeaker(participant, volume);
}

void ParticipantEventHandlerWrapper::onDisplayNameChanged(std::shared_ptr<vi::IParticipant> participant)
{
    emit displayNameChanged(participant);
}

void ParticipantEventHandlerWrapper::onCreateRemoteVideoTrack(std::shared_ptr<vi::IParticipant> participant, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    emit createRemoteVideoTrack(participant, tid, track);
}

void ParticipantEventHandlerWrapper::onRemoveRemoteVideoTrack(std::shared_ptr<vi::IParticipant> participant, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    emit removeRemoteVideoTrack(participant, tid, track);
}

void ParticipantEventHandlerWrapper::onRemoteAudioStateChanged(std::shared_ptr<vi::IParticipant> participant, bool muted)
{
    emit remoteAudioStateChanged(participant, muted);
}

void ParticipantEventHandlerWrapper::onRemoteVideoStateChanged(std::shared_ptr<vi::IParticipant> participant, bool muted)
{
    emit remoteVideoStateChanged(participant, muted);
}
