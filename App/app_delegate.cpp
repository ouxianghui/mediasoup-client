#include "app_delegate.h"
#include "service/component_factory.h"
#include "logger/u_logger.h"

AppDelegate::AppDelegate()
{

}

AppDelegate::~AppDelegate()
{
    DLOG("~AppDelegate()");
}

void AppDelegate::init()
{
    if (!_cf) {
        _cf = std::make_shared<vi::ComponentFactory>();
        _cf->init();
    }
}

void AppDelegate::destroy()
{
    if (_cf) {
        _cf->destroy();
    }
}

std::shared_ptr<vi::IComponentFactory> AppDelegate::getComponentFactory()
{
    return _cf;
}
