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

namespace vi
{
	class SDPUtils 
	{
	public:
		static std::vector<std::string> split(const std::string& str, char delim);

		static std::string join(const std::vector<std::string>& lines, const std::string& separator = "\n");

		static void injectSimulcast(int64_t simulcast, std::vector<std::string>& lines);
	};
}
