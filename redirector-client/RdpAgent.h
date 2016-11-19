#pragma once
#include "FreeRdp.h"
#include "RdpSetting.h"
#include <functional>
#include <memory>

class RdpAgent
{
public:
	enum Stage
	{ 
		CONNECTION_STATE_INITIAL = 0,
		CONNECTION_STATE_NEGO,
		CONNECTION_STATE_NLA,
		CONNECTION_STATE_MCS_CONNECT,
		CONNECTION_STATE_MCS_ERECT_DOMAIN,
		CONNECTION_STATE_MCS_ATTACH_USER,
		CONNECTION_STATE_MCS_CHANNEL_JOIN,
		CONNECTION_STATE_RDP_SECURITY_COMMENCEMENT,
		CONNECTION_STATE_SECURE_SETTINGS_EXCHANGE,
		CONNECTION_STATE_CONNECT_TIME_AUTO_DETECT,
		CONNECTION_STATE_LICENSING,
		CONNECTION_STATE_MULTITRANSPORT_BOOTSTRAPPING,
		CONNECTION_STATE_CAPABILITIES_EXCHANGE,
		CONNECTION_STATE_FINALIZATION,
		CONNECTION_STATE_ACTIVE
	};

	RdpAgent();
	~RdpAgent();

	void Connect(const RdpSetting &setting);
	void Disconnect();

	std::function<void()> DisconnectEvent;
	std::function<void(Stage)> StageChangedEvent;
	std::function<void(void*, unsigned int)> ScreenDataReceivedEvent;
	std::function<void(void*, unsigned int)> DRDataReceivedEvent;
	std::function<void(void*, unsigned int)> AudioDataReceivedEvent;
private:
	std::shared_ptr<FreeRdp> m_pFreerdp;
};

