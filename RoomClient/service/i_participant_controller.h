#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "utils/interface_proxy.hpp"

namespace vi {

class IParticipant;
class IParticipantControllerObserver;

using ParticipantMap = std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<IParticipant>>>;

class IParticipantController {
public:
    virtual ~IParticipantController() = default;

    virtual void init() = 0;

    virtual void destroy() = 0;

    virtual void addObserver(std::shared_ptr<IParticipantControllerObserver> observer) = 0;

    virtual void removeObserver(std::shared_ptr<IParticipantControllerObserver> observer) = 0;

    virtual std::shared_ptr<IParticipant> getParticipant(const std::string& pid) = 0;

    virtual std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<IParticipant>>> getParticipants() = 0;
};

BEGIN_PROXY_MAP(ParticipantController)
    PROXY_METHOD0(void, init)
    PROXY_METHOD0(void, destroy)
    PROXY_METHOD1(void, addObserver, std::shared_ptr<IParticipantControllerObserver>)
    PROXY_METHOD1(void, removeObserver, std::shared_ptr<IParticipantControllerObserver>)
    PROXY_METHOD1(std::shared_ptr<IParticipant>, getParticipant, const std::string&)
    PROXY_METHOD0(ParticipantMap, getParticipants)
END_PROXY_MAP()

}
