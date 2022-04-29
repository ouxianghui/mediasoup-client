#pragma once

#include <string>
#include "api/scoped_refptr.h"

namespace webrtc {
    class MediaStreamTrackInterface;
}

namespace vi {

    class IMediaControllerObserver {
    public:
        virtual ~IMediaControllerObserver() = default;

        virtual void onLocalAudioStateChanged(bool enabled, bool muted) = 0;

        virtual void onLocalVideoStateChanged(bool enabled) = 0;

        virtual void onRemoteAudioStateChanged(const std::string& pid, bool muted) = 0;

        virtual void onRemoteVideoStateChanged(const std::string& pid, bool muted) = 0;

        virtual void onCreateLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) = 0;

        virtual void onRemoveLocalVideoTrack(const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) = 0;

        virtual void onCreateRemoteAudioTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) = 0;

        virtual void onRemoveRemoteAudioTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) = 0;

        virtual void onCreateRemoteVideoTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) = 0;

        virtual void onRemoveRemoteVideoTrack(const std::string& pid, const std::string& tid, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track) = 0;
    };

}
