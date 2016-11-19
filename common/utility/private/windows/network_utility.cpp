#include "network_utility.h"
#include "target_os.h"
#include "ScopeGuard.h"

// for multi network card
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib,"Ws2_32.lib")
using namespace Titanium::TIRA;

std::vector<std::string> NetworkHelper::GetAllIpAddress()
{
	std::vector<std::string> result;

	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ON_SCOPE_EXIT([&]()
	{
		if (pAdapterInfo)
			free(pAdapterInfo);
	});

	ULONG ulOutBufLen = sizeof(PIP_ADAPTER_INFO);
	pAdapterInfo = (PIP_ADAPTER_INFO)malloc(sizeof(PIP_ADAPTER_INFO));

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);
		if (NULL == pAdapterInfo)
			return result;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_SUCCESS)
	{
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			if (pAdapter->Type == MIB_IF_TYPE_ETHERNET
				|| pAdapter->Type == IF_TYPE_IEEE80211
				|| pAdapter->Type == IF_TYPE_PPP)
			{
				MIB_IFROW mibIfRow;
				mibIfRow.dwIndex = pAdapter->Index;
				if (GetIfEntry(&mibIfRow) == NO_ERROR)
				{
					/* For APIPA address. If a DHCP server does not respond after several discover messages are broadcast,
					the Windows computer assigns itself a Class B (APIPA) address. The Internet Assigned Numbers Authority (IANA)
					has reserved 169.254.0.0-169.254.255.255 for Automatic Private IP Addressing. */
					if ((mibIfRow.dwOperStatus == IF_OPER_STATUS_CONNECTED
						|| mibIfRow.dwOperStatus == IF_OPER_STATUS_OPERATIONAL)
						&& strncmp(pAdapter->IpAddressList.IpAddress.String, "169", 3) != 0
						&& strcmp(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0") != 0)
						result.push_back(pAdapter->IpAddressList.IpAddress.String);
				}
			}
			pAdapter = pAdapter->Next;
		}
	}

	return result;
}

bool NetworkHelper::IsWirelessInterfaceExist()
{
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ON_SCOPE_EXIT([&]()
	{
		if (pAdapterInfo)
			free(pAdapterInfo);
	});

	ULONG ulOutBufLen = sizeof(PIP_ADAPTER_INFO);
	pAdapterInfo = (PIP_ADAPTER_INFO)malloc(sizeof(PIP_ADAPTER_INFO));

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);
		if (NULL == pAdapterInfo)
			return false;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_SUCCESS)
	{
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
		while (pAdapter)
		{
			if (pAdapter->Type == IF_TYPE_IEEE80211)
				return true;
			pAdapter = pAdapter->Next;
		}
	}

	return false;
}

std::string NetworkHelper::GetHostByName(const std::string hostName)
{
	hostent* remoteHost = gethostbyname(hostName.c_str());
	if (remoteHost == NULL)
		return "";

	struct in_addr addr;
	addr.s_addr = *(u_long*)remoteHost->h_addr_list[0];
	std::string remoteIP(inet_ntoa(addr));

	return remoteIP;
}

std::string NetworkHelper::GetHostName()
{
	char szHostName[255] = "\0";
	if (gethostname(szHostName, sizeof(szHostName)) != SOCKET_ERROR)
	{
		return szHostName;
	}
	return "";
}

const NetworkHelper& NetworkHelper::operator=(const NetworkHelper&)
{
	return *this;
}

NetworkHelper::NetworkHelper(const NetworkHelper&)
{

}

NetworkHelper::NetworkHelper(void)
{

}