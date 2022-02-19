#pragma once

#include <memory>
#include <mutex>

namespace vi {

template<typename T>
class Singleton {
public:
    virtual ~Singleton() {}
    static std::shared_ptr<T>& sharedInstance()
    {
        static std::shared_ptr<T> _instance;
        static std::once_flag ocf;
        std::call_once(ocf, [](){
            _instance.reset(new T);
        });
        return _instance;
    }
};

}
