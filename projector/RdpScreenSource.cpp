#include "RdpScreenSource.h"

RdpScreenSource::RdpScreenSource(std::shared_ptr<Titanium::IChannel> channel) :IRdpSource(channel)
{	
}
RdpScreenSource::~RdpScreenSource()
{
	Stop();
}

