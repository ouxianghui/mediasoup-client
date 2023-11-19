/************************************************************************
* @Copyright: 2021-2024
* @FileName:
* @Description: Open source mediasoup room client library
* @Version: 1.0.0
* @Author: Jackie Ou
* @CreateTime: 2021-10-1
*************************************************************************/

#pragma once

#include <string>
#include <vector>

namespace vi {
    class IConnectionObserver {
	public:
        virtual ~IConnectionObserver() {}

		virtual void onOpen() = 0;

		virtual void onFail(int errorCode, const std::string& reason) = 0;

		virtual void onClose( int closeCode, const std::string& reason) = 0;

		virtual bool onValidate() = 0;

		virtual void onTextMessage(const std::string& text) = 0;

		virtual void onBinaryMessage(const std::vector<uint8_t>& data) = 0;

		virtual bool onPing(const std::string& text) = 0;

		virtual void onPong(const std::string& text) = 0;

		virtual void onPongTimeout(const std::string& text) = 0;
	};
}
