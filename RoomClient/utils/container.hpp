/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include <memory>

namespace vi {
    template<typename Proxy, typename Impl>
    class ProxyImpl {
    public:
        ProxyImpl(std::shared_ptr<Proxy> proxy, std::weak_ptr<Impl> impl)
        : _proxy(proxy)
        , _impl(impl)
        {}

        std::shared_ptr<Proxy> proxy() { return _proxy; }

        std::shared_ptr<Impl> impl(){ return _impl.lock(); }

    private:
        std::shared_ptr<Proxy> _proxy;
        std::weak_ptr<Impl> _impl;
    };
}
