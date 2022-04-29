
#pragma once

#include <type_traits>
#include <list>
#include <algorithm>
#include "absl/types/any.h"
#include "absl/types/optional.h"
#include "rtc_base/deprecated/recursive_critical_section.h"
#include "rtc_base/thread.h"

namespace vi {
    template<typename Observer>
    class UniversalObservable {
    public:
        using observer_ptr = std::shared_ptr<Observer>;
        void addWeakObserver(const observer_ptr &observer, rtc::Thread* thread) {
            rtc::CritScope scope(&_criticalSection);
            if (!hasObserverInternal(observer)) {
                WeakObject wobj(observer, thread);
                _observers.emplace_back(wobj);
            }
        }
        
        void addObserver(const observer_ptr &observer, rtc::Thread* thread) {
            rtc::CritScope scope(&_criticalSection);
            if (!hasObserverInternal(observer)) {
                Object obj(observer, thread);
                _observers.emplace_back(obj);
            }
        }
        
        void removeObserver(const observer_ptr &observer) {
            rtc::CritScope scope(&_criticalSection);
            for (auto it = _observers.begin(); it != _observers.end(); ++it) {
                auto var = absl::any(*it);
                if (var.has_value()) {
                    if (absl::any_cast<WeakObject>(&var)) {
                        WeakObject wobj = absl::any_cast<WeakObject>(var);
                        if (wobj.observer.lock() == observer) {
                            _observers.erase(it);
                            return;
                        }
                    } else if (absl::any_cast<Object>(&var)) {
                        Object obj = absl::any_cast<Object>(var);
                        if (obj.observer == observer) {
                            _observers.erase(it);
                            return;
                        }
                    }
                }
            }
        }
        
        void clearObserver() {
            rtc::CritScope scope(&_criticalSection);
            _observers.clear();
        }
        
        size_t numOfObservers() {
            rtc::CritScope scope(&_criticalSection);
            return _observers.size();
        }
        
        bool hasObserver(const observer_ptr &observer) {
            rtc::CritScope scope(&_criticalSection);
            return hasObserverInternal(observer);
        }
        
    protected:
        bool hasObserverInternal(const observer_ptr &observer) {
            for (auto it = _observers.begin(); it != _observers.end(); ++it) {
				auto var = absl::any(*it);
                if (var.has_value()) {
                    if (absl::any_cast<WeakObject>(&var)) {
                        WeakObject wobj = absl::any_cast<WeakObject>(var);
                        if (wobj.observer.lock() == observer) {
                            return true;
                        }
                    } else if (absl::any_cast<Object>(&var)) {
                        Object obj = absl::any_cast<Object>(var);
                        if (obj.observer == observer) {
                            return true;
                        }
                    }
                }
            }
            
            return false;
        }
        
    protected:
        virtual void notifyObservers(std::function<void(const observer_ptr &)> notifier) const {
            decltype(_observers) observers;
            {
                rtc::CritScope scope(&_criticalSection);
                observers = _observers;
            }
            
            for (const auto &observer: observers) {
                auto var = absl::any(observer);
                if (var.has_value()) {
                    std::shared_ptr<Observer> obs;
                    rtc::Thread* thread = nullptr;
                    
                    if (absl::any_cast<WeakObject>(&var)) {
                        WeakObject wobj = absl::any_cast<WeakObject>(var);
                        obs = wobj.observer.lock();
                        thread = wobj.thread;
                    } else if (absl::any_cast<Object>(&var)) {
                        Object obj = absl::any_cast<Object>(var);
                        obs = obj.observer;
                        thread = obj.thread;
                    }
                    
                    if (obs) {
						assert(thread);
						if (thread->IsCurrent()) {
                            notifier(obs);
						}
                        else {
                            thread->PostTask(RTC_FROM_HERE, [wobs = std::weak_ptr<Observer>(obs), notifier]() {
								if (auto observer = wobs.lock()) {
                                    notifier(observer);
								}
							});
						}
                    }
                }
            }
        }
        
    private:
        template<typename T = std::shared_ptr<Observer>>
        class InnerObject {
        public:
            InnerObject(std::shared_ptr<Observer> o, rtc::Thread* t)
            : observer(o)
            , thread(t) {
            }
            
            T observer;
            rtc::Thread* thread;
        };
        
        using WeakObject = InnerObject<std::weak_ptr<Observer>>;
        using Object = InnerObject<std::shared_ptr<Observer>>;
        
        rtc::RecursiveCriticalSection _criticalSection;
        std::list<absl::any> _observers;
    };
}
