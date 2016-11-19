#pragma once
#include "IRdpSource.h"

class RdpScreenSource : public IRdpSource
{
public:
	RdpScreenSource(std::shared_ptr<Titanium::IChannel> channel);
	virtual ~RdpScreenSource();
};

