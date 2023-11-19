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
#include <string>

namespace vi {
    class ITransportObserver
	{
	public:
        virtual ~ITransportObserver() {}

		virtual void onOpened() = 0;

		virtual void onClosed() = 0;

		virtual void onFailed(int errorCode, const std::string& reason) = 0;

		virtual void onMessage(const std::string& json) = 0;

	};
}
