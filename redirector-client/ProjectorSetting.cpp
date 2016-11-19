#include "ProjectorSetting.h"

ProjectorSetting::ProjectorSetting(const std::string & ip, unsigned short negotiationPort, unsigned short mediaPort)
	: _ip(ip)
	, _negotiationPort(negotiationPort)
	, _mediaPort(mediaPort)
{
}

Titanium::TIRA::InetAddress ProjectorSetting::GetNegotiationAddress() const
{
	return Titanium::TIRA::InetAddress(_ip, _negotiationPort);
}

Titanium::TIRA::InetAddress ProjectorSetting::GetMeidaAddress() const
{
	return Titanium::TIRA::InetAddress(_ip, _mediaPort);
}
