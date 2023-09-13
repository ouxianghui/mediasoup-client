#include "component_factory.h"
#include "service_factory.hpp"
#include "utils/thread_provider.h"

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
        _threadProvider->create({ "signaling", "mediasoup", "capture" });
    }

    if (!_serviceFactory) {
        _serviceFactory = std::make_shared<ServiceFactory>(weak_from_this());
        _serviceFactory->init();
    }
}

void ComponentFactory::destroy()
{
    if (_serviceFactory) {
        _serviceFactory->destroy();
        _serviceFactory = nullptr;
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

}
