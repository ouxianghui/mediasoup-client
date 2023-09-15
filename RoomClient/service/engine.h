#pragma once

#include <memory>
#include <unordered_map>
#include "utils/singleton.h"

class Broadcaster;

namespace vi {
    class IRoomClient;
    class RTCContext;

    class Engine : public vi::Singleton<Engine>
    {
    public:
        ~Engine();

        void init();

        void destroy();

        void setRTCLoggingSeverity(const std::string& level = "error");

        std::shared_ptr<RTCContext> getRTCContext();

        std::shared_ptr<IRoomClient> createRoomClient();

        const std::unordered_map<std::string, std::shared_ptr<IRoomClient>>& getRoomClients();

        std::shared_ptr<Broadcaster> createBroadcaster();

        const std::unordered_map<std::string, std::shared_ptr<Broadcaster>>& getBroadcasters();

    private:
        Engine();

        Engine(Engine&&) = delete;

        Engine(const Engine&) = delete;

        Engine& operator=(const Engine&) = delete;

        Engine& operator=(Engine&&) = delete;

    private:
        friend class vi::Singleton<Engine>;

        std::shared_ptr<RTCContext> _rtcContext;

        std::unordered_map<std::string, std::shared_ptr<IRoomClient>> _roomClients;

        std::unordered_map<std::string, std::shared_ptr<Broadcaster>> _broadcasters;
    };
}

#define getEngine() vi::Engine::sharedInstance()

#define getRoomClient(ID) getEngine()->getRoomClients().find(ID) != getEngine()->getRoomClients().end() ? getEngine()->getRoomClients().at(ID) : nullptr

#define getBroadcaster(ID) getEngine()->getBroadcasters().find(ID) != getEngine()->getBroadcasters().end() ? getEngine()->getBroadcasters().at(ID) : nullptr
