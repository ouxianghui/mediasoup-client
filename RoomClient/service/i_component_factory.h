#pragma once

#include <memory>

namespace vi {

    class ThreadProvider;
    class IServiceFactory;
    class IRoomClient;

    class IComponentFactory {
    public:
        virtual ~IComponentFactory() {}

        virtual void init() = 0;

        virtual void destroy() = 0;

        virtual const std::unique_ptr<ThreadProvider>& getThreadProvider() = 0;

        virtual std::shared_ptr<IServiceFactory> getServiceFactory() = 0;
    };

}
