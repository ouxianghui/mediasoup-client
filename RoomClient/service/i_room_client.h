#pragma once

#include <memory>
#include <string>
#include "i_room_client_observer.h"
#include "utils/interface_proxy.hpp"
#include "options.h"

namespace vi {

class IParticipantController;
class IMediaController;

class IRoomClient {
public:
    virtual ~IRoomClient() = default;

    virtual void init() = 0;

    virtual void destroy() = 0;

    virtual void addObserver(std::shared_ptr<IRoomClientObserver> observer) = 0;

    virtual void removeObserver(std::shared_ptr<IRoomClientObserver> observer) = 0;

    virtual void join(const std::string& host, uint16_t port, const std::string& roomId, const std::string& displayName, std::shared_ptr<Options> opts) = 0;

    virtual void leave() = 0;

    virtual std::shared_ptr<IMediaController> getMediaController() = 0;

    virtual std::shared_ptr<IParticipantController> getParticipantController() = 0;
};

BEGIN_PROXY_MAP(RoomClient)
    PROXY_METHOD0(void, init)
    PROXY_METHOD0(void, destroy)
    PROXY_METHOD1(void, addObserver, std::shared_ptr<IRoomClientObserver>)
    PROXY_METHOD1(void, removeObserver, std::shared_ptr<IRoomClientObserver>)
    PROXY_METHOD5(void, join, const std::string&, uint16_t, const std::string&, const std::string&, std::shared_ptr<Options>)
    PROXY_METHOD0(void, leave)
    PROXY_METHOD0(std::shared_ptr<IMediaController>, getMediaController)
    PROXY_METHOD0(std::shared_ptr<IParticipantController>, getParticipantController)
END_PROXY_MAP()

}
