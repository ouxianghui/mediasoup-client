#pragma once

#include <memory>
#include <map>
#include "i_component_factory.h"
#include "i_service.hpp"

namespace vi {

class ComponentFactory : public IComponentFactory, public std::enable_shared_from_this<ComponentFactory>
{
public:
    ComponentFactory();

    ~ComponentFactory();

    void init() override;

    void destroy() override;

    const std::unique_ptr<ThreadProvider>& getThreadProvider() override;

    std::shared_ptr<IServiceFactory> getServiceFactory() override;

    std::shared_ptr<IRoomClient> getRoomClient() override;

private:
    ComponentFactory(ComponentFactory&&) = delete;

    ComponentFactory(const ComponentFactory&) = delete;

    const ComponentFactory& operator=(const ComponentFactory&) = delete;

private:
    std::unique_ptr<ThreadProvider> _threadProvider;

    std::shared_ptr<IServiceFactory> _serviceFactory;

    std::shared_ptr<IRoomClient> _roomClient;
};

}

