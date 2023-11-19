/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#include "engine.h"
#include "logger/spd_logger.h"
#include "rtc_context.hpp"
#include "room_client.h"
#include "broadcaster.hpp"
#include "component_factory.h"

namespace vi {
    Engine::Engine()
    {

    }

    Engine::~Engine()
    {
        DLOG("~Engine()");
    }

    void Engine::init()
    {
        //vi::Logger::init();

        if (!_rtcContext) {
            _rtcContext = std::make_shared<RTCContext>();
            _rtcContext->init();
        }

        //setRTCLoggingSeverity("error");
    }

    void Engine::destroy()
    {
        for (const auto& b : _broadcasters) {
            b.second->Stop();
        }

        for (const auto& c : _roomClients) {
            c.second->destroy();
        }

        if (_rtcContext) {
            _rtcContext->destroy();
        }

        //vi::Logger::destroy();
    }

    void Engine::setRTCLoggingSeverity(const std::string& level)
    {
        //Set RTC logging severity.
        if (level== "info") {
            rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_INFO);
        } else if (level == "warn") {
            rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_WARNING);
        } else if (level == "error") {
            rtc::LogMessage::LogToDebug(rtc::LoggingSeverity::LS_ERROR);
        }
    }

    std::shared_ptr<RTCContext> Engine::getRTCContext()
    {
        return _rtcContext;
    }

    std::shared_ptr<IRoomClient> Engine::createRoomClient()
    {
        rtc::Thread* mediasoupThread = getThread("mediasoup");
        rtc::Thread* transportThread = getThread("transport");

        auto RoomClientImpl = std::make_shared<RoomClient>(_rtcContext, mediasoupThread, transportThread);
        auto roomClient = RoomClientProxy::create(RoomClientImpl, mediasoupThread);
        roomClient->init();

        _roomClients[roomClient->getId()] = roomClient;

        return roomClient;
    }

    const std::unordered_map<std::string, std::shared_ptr<IRoomClient>>& Engine::getRoomClients()
    {
        return _roomClients;
    }

    std::shared_ptr<Broadcaster> Engine::createBroadcaster()
    {
        std::shared_ptr<Broadcaster> broadcaster = std::make_shared<Broadcaster>(_rtcContext);

        _broadcasters[broadcaster->getId()] = broadcaster;

        return broadcaster;
    }

    const std::unordered_map<std::string, std::shared_ptr<Broadcaster>>& Engine::getBroadcasters()
    {
        return _broadcasters;
    }
}
