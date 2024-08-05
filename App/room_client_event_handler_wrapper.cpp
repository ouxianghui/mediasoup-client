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
    Q_EMIT roomStateChanged(state);
}

void RoomClientEventHandlerWrapper::onCreateLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    Q_EMIT createLocalVideoTrack(tid, track);
}

void RoomClientEventHandlerWrapper::onRemoveLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
{
    Q_EMIT removeLocalVideoTrack(tid, track);
}

void RoomClientEventHandlerWrapper::onLocalAudioStateChanged(bool enabled, bool muted)
{
    Q_EMIT localAudioStateChanged(enabled, muted);
}

void RoomClientEventHandlerWrapper::onLocalVideoStateChanged(bool enabled)
{
    Q_EMIT localVideoStateChanged(enabled);
}

void RoomClientEventHandlerWrapper::onLocalActiveSpeaker(int32_t volume)
{
    Q_EMIT localActiveSpeaker(volume);
}
