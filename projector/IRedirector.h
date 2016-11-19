#pragma once
#include <string>
#include <memory>
#include "Socket.h"
#include "IChannel.h"
#include "RedirectPipe.h"
class IRedirector
{
public:
	IRedirector(std::shared_ptr<Titanium::IChannel> channel);
	virtual ~IRedirector();
	virtual void Start() = 0;
	virtual void Stop() = 0;
protected:
	std::shared_ptr<Titanium::IChannel> _RedirectChannel;
	std::shared_ptr<RedirectPipe> _Pipe;
	std::mutex m_mux;
};