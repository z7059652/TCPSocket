#pragma once
#include <vector>
#include <string>

namespace Titanium { namespace TIRA
{
	class NetworkHelper
	{
	public:
		static std::string GetHostName();
		static std::string GetHostByName(const std::string hostName);
		static std::vector<std::string> GetAllIpAddress();
		static bool IsWirelessInterfaceExist();
	private:
		NetworkHelper(void);
		NetworkHelper(const NetworkHelper&);
		const NetworkHelper& operator=(const NetworkHelper&);
	};
}
}
