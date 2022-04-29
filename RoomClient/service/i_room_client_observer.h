#pragma once

#include <string>
#include "api/scoped_refptr.h"

namespace webrtc {
    class MediaStreamTrackInterface;
}

namespace vi {

enum class RoomState {
    UNKNOWN = 0,
    CONNECTING,
    CONNECTED,
    CLOSED
};

class IRoomClientObserver {
public:
    virtual ~IRoomClientObserver() = default;

    virtual void onRoomStateChanged(RoomState state) = 0;

    virtual void onCreateLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>) = 0;

    virtual void onRemoveLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>) = 0;

    virtual void onLocalAudioStateChanged(bool enabled, bool muted) = 0;

    virtual void onLocalVideoStateChanged(bool enabled) = 0;

    virtual void onLocalActiveSpeaker(int32_t volume) = 0;
};

}
