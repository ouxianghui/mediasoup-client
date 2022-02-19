#pragma once

#include <string>

namespace webrtc {
    class MediaStreamTrackInterface;
}

namespace vi {

class IMediaControllerObserver {
public:
    virtual ~IMediaControllerObserver() = default;

    virtual void onCreateVideoTrack(const std::string& id, webrtc::MediaStreamTrackInterface* track) = 0;

    virtual void onRemoveVideoTrack(const std::string& id, webrtc::MediaStreamTrackInterface* track) = 0;
};

}
