#include "engine.h"
#include "service/component_factory.h"
#include "logger/u_logger.h"

namespace vi {
    Engine::Engine()
    {

    }

    Engine::~Engine()
    {
        DLOG("~Engine()");
    }

    void Engine::init()
    {
        if (!_cf) {
            _cf = std::make_shared<vi::ComponentFactory>();
            _cf->init();
        }
    }

    void Engine::destroy()
    {
        if (_cf) {
            _cf->destroy();
        }
    }

    std::shared_ptr<vi::IComponentFactory> Engine::getComponentFactory()
    {
        return _cf;
    }
}
