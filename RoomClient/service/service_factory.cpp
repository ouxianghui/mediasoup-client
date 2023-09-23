
#include "service_factory.hpp"
#include "i_service.hpp"

namespace vi
{
    ServiceFactory::ServiceFactory()
    {

    }

    void ServiceFactory::init()
    {
        _init();
    }

    void ServiceFactory::destroy()
    {
        _destroy();
    }

	void ServiceFactory::registerService(const std::string& key, const std::shared_ptr<IService>& service) 
    {
        ObjectFactory<IService>::_register(key, service);
	}

	void ServiceFactory::unregisterService(const std::string& key) 
    {
        ObjectFactory<IService>::_unregister(key);
	}
}
