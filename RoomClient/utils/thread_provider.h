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
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <string>
#include <list>
#include "singleton.h"

namespace rtc {
    class Thread;
}

namespace vi {

    class ThreadProvider
	{
	public:
		ThreadProvider();

		~ThreadProvider();

		void init();

		void destroy();

        void create(const std::list<std::string>& threadNames);

        rtc::Thread* thread(const std::string& name);

	private:
		ThreadProvider(const ThreadProvider&) = delete;

		ThreadProvider(ThreadProvider&&) = delete;

		ThreadProvider& operator=(const ThreadProvider&) = delete;

		ThreadProvider& operator=(ThreadProvider&&) = delete;

	public:
		void stopAll();

	private:
        std::unordered_map<std::string, rtc::Thread*> _threadsMap;
		
		std::mutex _mutex;

		rtc::Thread* _mainThread = nullptr;

		std::atomic_bool _inited;

		std::atomic_bool _destroy;
	};
}
