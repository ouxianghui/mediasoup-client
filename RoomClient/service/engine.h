#pragma once

#include <memory>
#include "utils/singleton.h"
#include "utils/thread_provider.h"
#include "service/component_factory.h"

namespace vi {
    class IComponentFactory;
    class Engine : public vi::Singleton<Engine>
    {
    public:
        ~Engine();

        void init();

        void destroy();

        std::shared_ptr<vi::IComponentFactory> getComponentFactory();

    private:
        Engine();

        Engine(Engine&&) = delete;

        Engine(const Engine&) = delete;

        Engine& operator=(const Engine&) = delete;

    private:
        friend class vi::Singleton<Engine>;

        std::shared_ptr<vi::IComponentFactory> _cf;
    };
}

#define getEngine() vi::Engine::sharedInstance()

#define getComponents() getEngine()->getComponentFactory()

#define getThread(T) getComponents()->getThreadProvider()->thread(T)

#define getService(S) getComponents()->getServiceFactory()->getService<S>()

#define getRoomClient() getComponents()->getRoomClient()
