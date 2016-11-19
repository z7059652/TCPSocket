#pragma once
#include "Component.h"
#include "ISource.h"
#include "IChannel.h"
class RdpStreamingParser;
class IRdpSource : public Component, public ISource
{
public:
	IRdpSource(std::shared_ptr<Titanium::IChannel> channel);
	virtual ~IRdpSource();
	void Start();
	void Stop();
protected:
	virtual void Process();
	std::shared_ptr<Titanium::IChannel> m_Channel;
private:
	std::shared_ptr<RdpStreamingParser>	m_Parser;
};

