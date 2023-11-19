/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include "i_participant.h"

namespace webrtc {
    class MediaStreamTrackInterface;
}

namespace vi {

    class Participant : public IParticipant
    {
    public:
        Participant(const std::string& id, const std::string& displayName);

        std::string id() override;

        std::string displayName() override;

        bool isActive() override;

        bool hasAudio() override;

        bool isAudioMuted() override;

        bool hasVideo() override;

        bool isVideoMuted() override;

        int32_t speakingVolume() override;

        std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>> getVideoTracks() override;

        void setId(const std::string& id);

        void setDisplayName(const std::string& name);

        void setActive(bool isActive);

        void setAudioMuted(bool muted);

        void setVideoMuted(bool muted);

        void setSpeakingVolume(int32_t volume);

        void setAudioTracks(const std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>>& tracks);

        void setVideoTracks(const std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>>& tracks);

    private:
        std::string _id;

        std::string _displayName;

        bool _isActive;

        bool _isAudioMuted;

        bool _isVideoMuted;

        int32_t _volume;

        std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>> _audioTracks;

        std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>> _videoTracks;
    };

}
