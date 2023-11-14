#include "room_client_event_handler_wrapper.h"
#include "api/media_stream_interface.h"


RoomClientEventHandlerWrapper::RoomClientEventHandlerWrapper(QObject* parent) : QObject(parent)
{

}

void RoomClientEventHandlerWrapper::init()
{

}

void RoomClientEventHandlerWrapper::destroy()
{

}

void RoomClientEventHandlerWrapper::onRoomStateChanged(vi::RoomState state)
{
    emit roomStateChanged(state);
}

void RoomClientEventHandlerWrapper::onCreateLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    emit createLocalVideoTrack(tid, track);
}

void RoomClientEventHandlerWrapper::onRemoveLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    emit removeLocalVideoTrack(tid, track);
}

void RoomClientEventHandlerWrapper::onLocalAudioStateChanged(bool enabled, bool muted)
{
    emit localAudioStateChanged(enabled, muted);
}

void RoomClientEventHandlerWrapper::onLocalVideoStateChanged(bool enabled)
{
    emit localVideoStateChanged(enabled);
}

void RoomClientEventHandlerWrapper::onLocalActiveSpeaker(int32_t volume)
{
    emit localActiveSpeaker(volume);
}
