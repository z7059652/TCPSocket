#pragma once
#include "IRdpSource.h"

class AudioSource : public IRdpSource
{
public:
	AudioSource(std::shared_ptr<Titanium::IChannel> channel);
	virtual ~AudioSource();
};

