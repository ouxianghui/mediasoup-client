/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/
#pragma warning(disable:4996)
#include "core.h"

#include "mediasoupclient.hpp"
#include "logger/spd_logger.h"
#include "service/engine.h"
#include "service/component_factory.h"
#include "service/service_factory.hpp"

namespace vi {

Core::Core()
{

}

void Core::init()
{
    vi::Logger::init();

    mediasoupclient::Initialize();

    getComponentFactory()->init();

    registerServices();

    getServiceFactory()->init();

    getEngine()->init();

    DLOG("mediasoupclient version: {}", mediasoupclient::Version().c_str());
}

void Core::destroy()
{
    mediasoupclient::Cleanup();

    getEngine()->destroy();

    getServiceFactory()->destroy();

    getComponentFactory()->destroy();

    vi::Logger::destroy();
}

void Core::registerServices()
{
    //getServiceFactory()->registerService(typeid(vi::HostService).name(), std::make_shared<vi::HostService>());
    //getServiceFactory()->registerService(typeid(vi::SharingService).name(), std::make_shared<vi::SharingService>());
}

}
