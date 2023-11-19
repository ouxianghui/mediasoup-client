/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include "rtc_base/logging.h"

namespace vi
{
	class RTCLogSink : public rtc::LogSink
	{
	public:
		void OnLogMessage(const std::string& msg, rtc::LoggingSeverity severity, const char* tag) override;

		void OnLogMessage(const std::string& message, rtc::LoggingSeverity severity) override;

		void OnLogMessage(const std::string& message) override;
	};

}
