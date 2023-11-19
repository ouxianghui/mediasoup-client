/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

﻿#pragma once

#include <memory>
#include <functional>
#include <algorithm>
#include <vector>

namespace vi {

using namespace std;

class Observable
{
public:
    virtual ~Observable(){}

protected:
    template<class T>
    void addObserver(std::vector<weak_ptr<T>> &observers, weak_ptr<T> observer)
    {
        auto lObserver = observer.lock();
        if(lObserver) {
            observers.push_back(observer);
        }
    }

    template<class T> void removeObserver(std::vector<weak_ptr<T>> &observers, weak_ptr<T> observer)
    {
        typename std::vector<weak_ptr<T>>::iterator iter = observers.begin();
        auto lockObserver = observer.lock();
        if (lockObserver == nullptr) {
            return;
        }

        while (iter != observers.end()) {
            if (iter->expired()) {
                iter = observers.erase(iter);
                continue;
            }
            if (iter->lock().get() == lockObserver.get()) {
                break;
            }
            ++iter;
        }
        if (iter != observers.end()) {
            observers.erase(iter);
        }
    }

    template<class T>
    void removeInvalid(std::vector<std::weak_ptr<T>> &observers)
    {
        observers.erase( std::remove_if(observers.begin(), observers.end(), [](const std::weak_ptr<T>& observer) {
                                            return observer.expired();
                                        }), observers.end() );
    }

    template<class T>
    void notify(std::vector<std::weak_ptr<T>> &observers, std::function<void(std::shared_ptr<T>& ot)> func)
    {
        removeInvalidObserver(observers);
        auto copiedObservers = observers;
        for(auto it = copiedObservers.begin(); it != copiedObservers.end(); ++it) {
            auto observer = (*it).lock();
            if (observer) {
                func(observer);
            }
        }
    }
};

}
