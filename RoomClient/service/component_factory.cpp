#include "component_factory.h"
#include "service_factory.hpp"
#include "utils/thread_provider.h"
#include "service/room_client.h"

namespace vi {

    ComponentFactory::ComponentFactory()
    {

    }

    ComponentFactory::~ComponentFactory() {

    }

    void ComponentFactory::init()
    {
        if (!_threadProvider) {
            _threadProvider = std::make_unique<ThreadProvider>();
            _threadProvider->init();
            _threadProvider->create({ "signaling-transport", "mediasoup-client", "capture-session", "main" });
        }

        if (!_serviceFactory) {
            _serviceFactory = std::make_shared<ServiceFactory>(weak_from_this());
            _serviceFactory->init();
        }

        if (!_roomClient) {
            rtc::Thread* internalThread = _threadProvider->thread("mediasoup-client");
            rtc::Thread* transportThread = _threadProvider->thread("signaling-transport");
            auto RoomClientImpl = std::make_shared<RoomClient>(weak_from_this(), internalThread, transportThread);
            _roomClient = RoomClientProxy::create(RoomClientImpl, internalThread);
            _roomClient->init();
        }
    }

    void ComponentFactory::destroy()
    {
        if (_serviceFactory) {
            _serviceFactory->destroy();
            _serviceFactory = nullptr;
        }

        if (_roomClient) {
            _roomClient->destroy();
            _roomClient = nullptr;
        }

        if (_threadProvider) {
            _threadProvider->destroy();
            _threadProvider = nullptr;
        }
    }

    const std::unique_ptr<ThreadProvider>& ComponentFactory::getThreadProvider()
    {
        return _threadProvider;
    }

    std::shared_ptr<vi::IServiceFactory> ComponentFactory::getServiceFactory()
    {
        return _serviceFactory;
    }

    std::shared_ptr<IRoomClient> ComponentFactory::getRoomClient()
    {
        return _roomClient;
    }

}
