/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#include "component_factory.h"
#include "utils/thread_provider.h"
#include "utils/notification_center.hpp"
#include "network/network_request_manager.h"
#include "network/network_http_client.h"
#include "network/network_request_plugin.h"
#include "network/network_request_consumer.h"

namespace vi {

    ComponentFactory::ComponentFactory()
    {

    }

    void ComponentFactory::init()
    {
        if (!_threadProvider) {
            _threadProvider = std::make_unique<ThreadProvider>();
            _threadProvider->init();
#ifdef WIN32
            _threadProvider->create({ "transport", "mediasoup", "communication"});
#else
            _threadProvider->create({ "transport", "mediasoup", "communication", "main" });
#endif
        }

        if (!_networkRequestManager) {
            _networkRequestManager = std::make_unique<NetworkRequestManager>();
            _networkRequestManager->init();
        }

        auto plugin = std::make_shared<NetworkRequestPlugin>();
        auto httpClient = std::make_shared<NetworkHttpClient>();
        auto consumer = std::make_shared<NetworkRequestConsumer>(plugin, httpClient, 5, RequestRoute::HTTP);
        plugin->addRequestConsumer(RequestRoute::HTTP, consumer);
        _networkRequestManager->registerPlugin("universal", plugin);
    }

    void ComponentFactory::destroy()
    {
        if (_threadProvider) {
            _threadProvider->destroy();
            _threadProvider = nullptr;
        }
    }

    const std::unique_ptr<ThreadProvider>& ComponentFactory::getThreadProvider()
    {
        return _threadProvider;
    }

    const std::unique_ptr<NotificationCenter>& ComponentFactory::getNotificationCenter()
    {
        return NotificationCenter::defaultCenter();
    }

    const std::unique_ptr<INetworkRequestManager>& ComponentFactory::getNetworkRequestManager()
    {
        return _networkRequestManager;
    }

}
