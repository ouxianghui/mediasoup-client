#pragma once

#include <memory>
#include <string>
#include "i_room_client_observer.h"
#include "utils/interface_proxy.hpp"
#include "options.h"

namespace rtc {
    class Thread;
}

namespace webrtc {
    class MediaStreamTrackInterface;
}

namespace vi {

class IParticipantController;

class IRoomClient {
public:
    virtual ~IRoomClient() = default;

    virtual void init() = 0;

    virtual void destroy() = 0;

    virtual std::string getId() = 0;

    virtual void addObserver(std::shared_ptr<IRoomClientObserver> observer, rtc::Thread* callbackThread) = 0;

    virtual void removeObserver(std::shared_ptr<IRoomClientObserver> observer) = 0;

    virtual void join(const std::string& host, uint16_t port, const std::string& roomId, const std::string& displayName, std::shared_ptr<Options> opts) = 0;

    virtual void leave() = 0;

    virtual RoomState getRoomState() = 0;

    virtual void enableAudio(bool enabled) = 0;

    virtual bool isAudioEnabled() = 0;

    virtual void muteAudio(bool muted) = 0;

    virtual bool isAudioMuted() = 0;

    virtual void enableVideo(bool enabled) = 0;

    virtual bool isVideoEnabled() = 0;

    virtual int32_t speakingVolume() = 0;

    virtual std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>> getVideoTracks() = 0;

    virtual std::shared_ptr<IParticipantController> getParticipantController() = 0;
};

using TrackMap = std::unordered_map<std::string, rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>>;

BEGIN_PROXY_MAP(RoomClient)
    PROXY_METHOD0(void, init)
    PROXY_METHOD0(void, destroy)
    PROXY_METHOD0(std::string, getId)
    PROXY_METHOD2(void, addObserver, std::shared_ptr<IRoomClientObserver>, rtc::Thread*)
    PROXY_METHOD1(void, removeObserver, std::shared_ptr<IRoomClientObserver>)
    PROXY_METHOD5(void, join, const std::string&, uint16_t, const std::string&, const std::string&, std::shared_ptr<Options>)
    PROXY_METHOD0(void, leave)
    PROXY_METHOD0(RoomState, getRoomState)
    PROXY_METHOD1(void, enableAudio,  bool)
    PROXY_METHOD0(bool, isAudioEnabled)
    PROXY_METHOD1(void, muteAudio, bool)
    PROXY_METHOD0(bool, isAudioMuted)
    PROXY_METHOD1(void, enableVideo, bool)
    PROXY_METHOD0(bool, isVideoEnabled)
    PROXY_METHOD0(TrackMap, getVideoTracks)
    PROXY_METHOD0(int32_t, speakingVolume)
    PROXY_METHOD0(std::shared_ptr<IParticipantController>, getParticipantController)
END_PROXY_MAP()

}
