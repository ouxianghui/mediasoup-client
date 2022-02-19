#pragma once

#include <memory>
#include "utils/interface_proxy.hpp"

namespace vi {

class IMediaControllerObserver;

class IMediaController {
public:
    virtual ~IMediaController() = default;

    virtual void init() = 0;

    virtual void destroy() = 0;

    virtual void addObserver(std::shared_ptr<IMediaControllerObserver> observer) = 0;

    virtual void removeObserver(std::shared_ptr<IMediaControllerObserver> observer) = 0;

    virtual void enableAudio(bool enabled) = 0;

    virtual bool isAudioEnabled() = 0;

    virtual void muteAudio(bool muted) = 0;

    virtual bool isAudioMuted() = 0;

    virtual void enableVideo(bool enabled) = 0;

    virtual bool isVideoEnabled() = 0;

    virtual void muteAudio(const std::string& id, bool muted) = 0;

    virtual bool isAudioMuted(const std::string& id) = 0;
};

BEGIN_PROXY_MAP(MediaController)
    PROXY_METHOD0(void, init)
    PROXY_METHOD0(void, destroy)
    PROXY_METHOD1(void, addObserver, std::shared_ptr<IMediaControllerObserver>)
    PROXY_METHOD1(void, removeObserver, std::shared_ptr<IMediaControllerObserver>)
    PROXY_METHOD1(void, enableAudio, bool)
    PROXY_METHOD0(bool, isAudioEnabled)
    PROXY_METHOD1(void, muteAudio, bool)
    PROXY_METHOD0(bool, isAudioMuted)
    PROXY_METHOD1(void, enableVideo, bool)
    PROXY_METHOD0(bool, isVideoEnabled)
    PROXY_METHOD2(void, muteAudio, const std::string&, bool)
    PROXY_METHOD1(bool, isAudioMuted, const std::string&)
END_PROXY_MAP()

}
