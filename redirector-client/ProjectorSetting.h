#pragma once
#include "Socket.h"

class ProjectorSetting
{
public:
	ProjectorSetting(const std::string &ip, unsigned short negotiationPort, unsigned short mediaPort);

	Titanium::TIRA::InetAddress GetNegotiationAddress() const;
	Titanium::TIRA::InetAddress GetMeidaAddress() const;
private:
	std::string _ip;
	unsigned short _negotiationPort;
	unsigned short _mediaPort;
};

