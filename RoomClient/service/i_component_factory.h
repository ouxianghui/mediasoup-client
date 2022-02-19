#pragma once

#include <memory>

namespace vi {

    class IServiceFactory;
    class IRoomClient;

    class IComponentFactory {
    public:
        virtual ~IComponentFactory() {}

        virtual void init() = 0;

        virtual void destroy() = 0;

        virtual std::shared_ptr<IServiceFactory> getServiceFactory() = 0;

        virtual std::shared_ptr<IRoomClient> getRoomClient() = 0;
    };

}
