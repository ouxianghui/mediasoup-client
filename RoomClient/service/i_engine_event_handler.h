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

	enum class EngineStatus {
		CONNECTED = 0,
		DISCONNECTED
	};

	class IEngineEventHandler
	{
	public:
		virtual ~IEngineEventHandler() {}

		virtual void onStatus(EngineStatus status) = 0;

		virtual void onError(int32_t code) = 0;
	};
}
