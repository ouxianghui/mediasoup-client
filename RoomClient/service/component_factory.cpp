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
    TMgr->init();
    TMgr->create({ "signaling-transport", "mediasoup-client", "capture-session" });
    _threadProvider = TMgr;

    if (!_serviceFactory) {
        _serviceFactory = std::make_shared<ServiceFactory>(weak_from_this());
        _serviceFactory->init();
    }

    if (!_roomClient) {
        auto RoomClientImpl = std::make_shared<RoomClient>(weak_from_this());
        _roomClient = RoomClientProxy::create(RoomClientImpl, "mediasoup-client");
        _roomClient->init();
    }
}

void ComponentFactory::destroy()
{
    if (_serviceFactory) {
        _serviceFactory->destroy();
    }

    if (_roomClient) {
        _roomClient->destroy();
    }
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
