/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/


#include "notification_center.hpp"
#include <type_traits>
#include <algorithm>
#include "i_observer.hpp"
#include "absl/types/optional.h"
#include "rtc_base/thread.h"

namespace {
    static std::unique_ptr<vi::NotificationCenter> _defaultCenter = std::make_unique<vi::NotificationCenter>();
}

namespace vi {

    NotificationCenter::NotificationCenter()
    {
    }

    void NotificationCenter::addObserver(const IObserver& observer)
    {
        rtc::CritScope scope(&_criticalSection);
        if (!hasObserverInternal(observer)) {
            _observers.emplace_back(observer.clone());
        }
    }

    void NotificationCenter::removeObserver(const IObserver& observer)
    {
        rtc::CritScope scope(&_criticalSection);
        for (auto it = _observers.begin(); it != _observers.end(); ++it) {
            if (observer.equals(**it)) {
                _observers.erase(it);
                return;
            }
        }
    }

    bool NotificationCenter::hasObserver(const IObserver& observer)
    {
        rtc::CritScope scope(&_criticalSection);
        return hasObserverInternal(observer);
    }

    void NotificationCenter::postNotification(const std::shared_ptr<INotification>& notification)
    {
        notifyObservers(notification);
    }

    void NotificationCenter::clearObserver()
    {
        rtc::CritScope scope(&_criticalSection);
        _observers.clear();
    }

    std::size_t NotificationCenter::numOfObservers()
    {
        rtc::CritScope scope(&_criticalSection);
        return _observers.size();
    }

    bool NotificationCenter::hasObserverInternal(const IObserver& observer)
    {
        for (auto it = _observers.begin(); it != _observers.end(); ++it) {
            if (observer.equals(**it)) {
                return true;
            }
        }
        return false;
    }

    void NotificationCenter::removeInvalidObservers()
    {
        _observers.erase(std::remove_if(_observers.begin(), _observers.end(), [](const auto& observer) {
            return !observer->isValid();
            }), _observers.end());
    }

    void NotificationCenter::notifyObservers(const std::shared_ptr<INotification>& notification)
    {
        decltype(_observers) observers;
        {
            rtc::CritScope scope(&_criticalSection);
            removeInvalidObservers();
            observers = _observers;
        }

        for (const auto& observer : observers) {
            if (observer && observer->shouldAccept(notification)) {
                rtc::Thread* thread = observer->scheduleThread();
                assert(thread);
                if (thread->IsCurrent()) {
				    observer->notify(notification);
                }
                else {
                    thread->PostTask([obs = std::weak_ptr<IObserver>(observer), notification]() {
						if (auto observer = obs.lock()) {
							observer->notify(notification);
						}
					});
                }
            }
        }
    }

    const std::unique_ptr<NotificationCenter>& NotificationCenter::defaultCenter()
    {
        return _defaultCenter;
    }

}
