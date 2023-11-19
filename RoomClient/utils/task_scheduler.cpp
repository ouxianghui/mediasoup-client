/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#include "task_scheduler.h"
#include "rtc_base/thread.h"

namespace vi {
	std::shared_ptr<TaskScheduler> TaskScheduler::create()
	{
		return std::shared_ptr<TaskScheduler>(new TaskScheduler(), [thread = rtc::Thread::Current()](TaskScheduler* ptr){
            thread->PostTask([ptr]() {
				delete ptr;
			});
		});
	}
}
