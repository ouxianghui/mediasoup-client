#pragma once

#include <QObject>
#include <memory>
#include "service/i_media_controller_observer.h"
#include "api/media_stream_interface.h"

class MediaEventAdapter : public QObject, public vi::IMediaControllerObserver, public std::enable_shared_from_this<MediaEventAdapter>
{
    Q_OBJECT
public:
    explicit MediaEventAdapter(QObject *parent = nullptr);

protected:
    void onCreateVideoTrack(const std::string& id, webrtc::MediaStreamTrackInterface* track) override;

    void onRemoveVideoTrack(const std::string& id, webrtc::MediaStreamTrackInterface* track) override;

signals:
    void videoTrackCreated(const std::string& id, webrtc::MediaStreamTrackInterface* track);

    void videoTrackRemoved(const std::string& id, webrtc::MediaStreamTrackInterface* track);
};

