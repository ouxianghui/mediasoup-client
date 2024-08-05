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
    Q_EMIT participantJoin(participant);
}

void ParticipantEventHandlerWrapper::onParticipantLeave(std::shared_ptr<vi::IParticipant> participant)
{
    Q_EMIT participantLeave(participant);
}

void ParticipantEventHandlerWrapper::onRemoteActiveSpeaker(std::shared_ptr<vi::IParticipant> participant, int32_t volume)
{
    Q_EMIT remoteActiveSpeaker(participant, volume);
}

void ParticipantEventHandlerWrapper::onDisplayNameChanged(std::shared_ptr<vi::IParticipant> participant)
{
    Q_EMIT displayNameChanged(participant);
}

void ParticipantEventHandlerWrapper::onCreateRemoteVideoTrack(std::shared_ptr<vi::IParticipant> participant, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    Q_EMIT createRemoteVideoTrack(participant, tid, track);
}

void ParticipantEventHandlerWrapper::onRemoveRemoteVideoTrack(std::shared_ptr<vi::IParticipant> participant, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    Q_EMIT removeRemoteVideoTrack(participant, tid, track);
}

void ParticipantEventHandlerWrapper::onRemoteAudioStateChanged(std::shared_ptr<vi::IParticipant> participant, bool muted)
{
    Q_EMIT remoteAudioStateChanged(participant, muted);
}

void ParticipantEventHandlerWrapper::onRemoteVideoStateChanged(std::shared_ptr<vi::IParticipant> participant, bool muted)
{
    Q_EMIT remoteVideoStateChanged(participant, muted);
}
