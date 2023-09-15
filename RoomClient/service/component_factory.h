#pragma once

#include <memory>
#include <map>
#include "i_service.hpp"
#include "utils/singleton.h"
#include "utils/thread_provider.h"
#include "network/i_network_request_manager.h"

namespace vi {

    class NotificationCenter;

    class ComponentFactory : public vi::Singleton<ComponentFactory>
    {
    public:
        void init();

        void destroy();

        const std::unique_ptr<ThreadProvider>& getThreadProvider();

        const std::unique_ptr<NotificationCenter>& getNotificationCenter();

        const std::unique_ptr<INetworkRequestManager>& getNetworkRequestManager();

    private:
        ComponentFactory();

        ComponentFactory(ComponentFactory&&) = delete;

        ComponentFactory(const ComponentFactory&) = delete;

        ComponentFactory& operator=(const ComponentFactory&) = delete;

        ComponentFactory& operator=(ComponentFactory&&) = delete;

    private:
        friend class vi::Singleton<ComponentFactory>;

        std::unique_ptr<ThreadProvider> _threadProvider;

        std::unique_ptr<INetworkRequestManager> _networkRequestManager;
    };
}

#define getComponentFactory() vi::ComponentFactory::sharedInstance()

#define getThread(T) getComponentFactory()->getThreadProvider()->thread(T)



