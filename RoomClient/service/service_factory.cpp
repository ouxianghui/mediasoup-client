
#include "service_factory.hpp"
#include "i_service.hpp"
#include "i_component_factory.h"

namespace vi
{
    ServiceFactory::ServiceFactory(std::weak_ptr<IComponentFactory> wcf)
        : _wcf(wcf)
    {

    }

    void ServiceFactory::init()
    {
        destroyObjects();
        
        initObjects();
    }

    void ServiceFactory::destroy()
    {
        destroyObjects();
    }

	void ServiceFactory::registerService(const std::string& key, const std::shared_ptr<IService>& service) 
    {
		ObjectFactory<IService>::registerObject(key, service);
	}

	void ServiceFactory::unregisterService(const std::string& key) 
    {
		ObjectFactory<IService>::unregisterObject(key);
	}
}
