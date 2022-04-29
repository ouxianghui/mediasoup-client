#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include "api/scoped_refptr.h"

namespace webrtc {
    class MediaStreamTrackInterface;
}

namespace vi {

class IParticipant {
public:
    virtual ~IParticipant() = default;

    virtual std::string id() = 0;

    virtual std::string displayName() = 0;

    virtual bool isActive() = 0;

    virtual bool hasAudio() = 0;

    virtual bool isAudioMuted() = 0;

    virtual bool hasVideo() = 0;

    virtual bool isVideoMuted() = 0;

    virtual int32_t speakingVolume() = 0;

    virtual std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>> getVideoTracks() = 0;
};

}
