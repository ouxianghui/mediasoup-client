/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "utils/interface_proxy.hpp"

namespace rtc {
    class Thread;
}

namespace vi {

class IParticipant;
class IParticipantEventHandler;

using ParticipantMap = std::unordered_map<std::string, std::shared_ptr<IParticipant>>;

class IParticipantController {
public:
    virtual ~IParticipantController() = default;

    virtual void addObserver(std::shared_ptr<IParticipantEventHandler> observer, rtc::Thread* callbackThread) = 0;

    virtual void removeObserver(std::shared_ptr<IParticipantEventHandler> observer) = 0;

    virtual std::shared_ptr<IParticipant> getParticipant(const std::string& pid) = 0;

    virtual std::unordered_map<std::string, std::shared_ptr<IParticipant>> getParticipants() = 0;

    virtual void muteAudio(const std::string& pid, bool muted) = 0;

    virtual bool isAudioMuted(const std::string& pid) = 0;

    virtual void muteVideo(const std::string& pid, bool muted) = 0;

    virtual bool isVideoMuted(const std::string& pid) = 0;

};

BEGIN_PROXY_MAP(ParticipantController)
    PROXY_METHOD2(void, addObserver, std::shared_ptr<IParticipantEventHandler>, rtc::Thread*)
    PROXY_METHOD1(void, removeObserver, std::shared_ptr<IParticipantEventHandler>)
    PROXY_METHOD1(std::shared_ptr<IParticipant>, getParticipant, const std::string&)
    PROXY_METHOD0(ParticipantMap, getParticipants)
    PROXY_METHOD2(void, muteAudio, const std::string&, bool)
    PROXY_METHOD1(bool, isAudioMuted, const std::string&)
    PROXY_METHOD2(void, muteVideo, const std::string&, bool)
    PROXY_METHOD1(bool, isVideoMuted, const std::string&)
END_PROXY_MAP()

}
