
#pragma once

#include <memory>
#include "utils/object_factory.hpp"
#include "utils/singleton.h"

namespace vi
{
    class IService;

    class ServiceFactory : public ObjectFactory<IService>, public Singleton<ServiceFactory>
    {
    public:
        void init();
        
        void destroy(); 

        void registerService(const std::string& key, const std::shared_ptr<IService>& service);

        void unregisterService(const std::string& key);

        template<class T>
        std::shared_ptr<T> getService() {
            return std::dynamic_pointer_cast<T>(getObject(typeid(T).name()));
        }

    private:
        ServiceFactory();

        ServiceFactory(ServiceFactory&&) = delete;

        ServiceFactory(const ServiceFactory&) = delete;

        ServiceFactory& operator=(const ServiceFactory&) = delete;

        ServiceFactory& operator=(ServiceFactory&&) = delete;

    private:
        friend class vi::Singleton<ServiceFactory>;
    };
}

#define getServiceFactory() vi::ServiceFactory::sharedInstance()

#define getService(S) getServiceFactory()->getService<S>()
