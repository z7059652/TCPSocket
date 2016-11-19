#pragma once
#include "IRdpSource.h"

class RdpDRSource : public IRdpSource
{
public:
	RdpDRSource(std::shared_ptr<Titanium::IChannel> channel);
	virtual ~RdpDRSource();
};

