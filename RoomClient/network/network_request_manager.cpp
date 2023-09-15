#include "network_request_manager.h"
#include "network_request_plugin.h"
#include "network_http_client.h"
#include "network_request_consumer.h"
#include "network_status_detector.h"

namespace vi {

NetworkRequestManager::NetworkRequestManager()
    : _plugins()
{

}

NetworkRequestManager::~NetworkRequestManager()
{

}

void NetworkRequestManager::init()
{
    //NetworkStatusDetector::instance()->init();
}

void NetworkRequestManager::registerPlugin(const std::string& name, std::shared_ptr<INetworkRequestHandler> plugin)
{
    std::lock_guard<std::mutex> guard(_lock);
	if (_plugins.find(name) == _plugins.end()) {
		_plugins[name] = plugin;
	}
}

void NetworkRequestManager::unregisterPlugin(const std::string& name)
{
    std::lock_guard<std::mutex> guard(_lock);
	if (_plugins.find(name) != _plugins.end()) {
		_plugins.erase(name);
	}
}

void NetworkRequestManager::addRequest(const std::shared_ptr<NetworkRequest>& request, bool isTail)
{
    std::shared_ptr<INetworkRequestHandler> handler;
    
    {
        std::lock_guard<std::mutex> guard(_lock);
        handler = _plugins[request->pluginName()];
    }

    if (handler) {
        handler->addRequest(request, isTail);
    }
}

void NetworkRequestManager::cancelAll()
{
    std::lock_guard<std::mutex> guard(_lock);

    for (auto it = _plugins.begin(); it != _plugins.end(); ++it) {
        (it->second)->cancelAll();
    }
}

void NetworkRequestManager::cancelAll(const std::string& pluginName)
{
    std::shared_ptr<INetworkRequestHandler> handler;

    {
        std::lock_guard<std::mutex> guard(_lock);
        handler = _plugins[pluginName];
    }

    if (handler) {
        handler->cancelAll();
    }
}

void NetworkRequestManager::cancelRequest(const std::shared_ptr<NetworkRequest>& request)
{
    std::shared_ptr<INetworkRequestHandler> handler;

    {
        std::lock_guard<std::mutex> guard(_lock);
        handler = _plugins[request->pluginName()];
    }

    if (handler) {
        handler->cancelRequest(request);
    }
}

}
