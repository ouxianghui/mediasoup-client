#include "media_event_adapter.h"

MediaEventAdapter::MediaEventAdapter(QObject *parent)
    : QObject{parent}
{

}

void MediaEventAdapter::onCreateVideoTrack(const std::string& id, webrtc::MediaStreamTrackInterface* track)
{
    emit videoTrackCreated(id, track);
}

void MediaEventAdapter::onRemoveVideoTrack(const std::string& id, webrtc::MediaStreamTrackInterface* track)
{
    emit videoTrackRemoved(id, track);
}
