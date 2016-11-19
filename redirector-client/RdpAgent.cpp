#include "RdpAgent.h"
#include <thread>

RdpAgent::RdpAgent() :
	m_pFreerdp(std::make_shared<FreeRdp>())
{
}

RdpAgent::~RdpAgent()
{
	Disconnect();
}

void RdpAgent::Connect(const RdpSetting &setting)
{
	m_pFreerdp->start((char *)setting.ToString().c_str());
}

void RdpAgent::Disconnect()
{
	m_pFreerdp->stop();
//	strcat()
}
