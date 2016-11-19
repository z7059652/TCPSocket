#include "RdpDRSource.h"

RdpDRSource::RdpDRSource(std::shared_ptr<Titanium::IChannel> channel) :IRdpSource(channel)
{
}
RdpDRSource::~RdpDRSource()
{
	Stop();
}
