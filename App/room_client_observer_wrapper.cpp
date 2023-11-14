#include "room_client_observer_wrapper.h"
#include "api/media_stream_interface.h"


RoomClientObserverWrapper::RoomClientObserverWrapper(QObject* parent) : QObject(parent)
{

}

void RoomClientObserverWrapper::init()
{

}

void RoomClientObserverWrapper::destroy()
{

}

void RoomClientObserverWrapper::onRoomStateChanged(vi::RoomState state)
{
    emit roomStateChanged(state);
}

void RoomClientObserverWrapper::onCreateLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    emit createLocalVideoTrack(tid, track);
}

void RoomClientObserverWrapper::onRemoveLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    emit removeLocalVideoTrack(tid, track);
}

void RoomClientObserverWrapper::onLocalAudioStateChanged(bool enabled, bool muted)
{
    emit localAudioStateChanged(enabled, muted);
}

void RoomClientObserverWrapper::onLocalVideoStateChanged(bool enabled)
{
    emit localVideoStateChanged(enabled);
}

void RoomClientObserverWrapper::onLocalActiveSpeaker(int32_t volume)
{
    emit localActiveSpeaker(volume);
}
