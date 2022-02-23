/**
 * This file is part of mediasoup_client project.
 * Author:    Jackie Ou
 * Created:   2021-11-01
 **/

#pragma once

#include <string>

namespace vi {
	class StringUtils
	{
	public:
		// Helper method to create random identifiers (e.g., transaction)
		static std::string randomString(int32_t len);
	};
}
