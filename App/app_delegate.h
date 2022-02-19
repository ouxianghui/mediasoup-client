#pragma once

#include <memory>
#include "utils/singleton.h"

namespace vi {
    class IComponentFactory;
}

class AppDelegate : public vi::Singleton<AppDelegate>
{
public:
    ~AppDelegate();

    void init();

    void destroy();

    std::shared_ptr<vi::IComponentFactory> getComponentFactory();

private:
    AppDelegate();

    AppDelegate(AppDelegate&&) = delete;

    AppDelegate(const AppDelegate&) = delete;

    AppDelegate& operator=(const AppDelegate&) = delete;

private:
    friend class vi::Singleton<AppDelegate>;

    std::shared_ptr<vi::IComponentFactory> _cf;
};

#define AppDSI AppDelegate::sharedInstance()
#define CFactory AppDSI->getComponentFactory()
#define FetchService(S) CFactory->getServiceFactory()->getService<S>()
#define RClient CFactory->getRoomClient()
