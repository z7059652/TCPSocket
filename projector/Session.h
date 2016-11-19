#pragma once
#include <memory>
#include <functional>
#include "SocketTcp.h"
class RedirectChannelFactory;
class RtspSession;
class Session
{
public:
	Session(std::shared_ptr<Titanium::TIRA::SocketTcp> socket);
	~Session();

	void Start();
	void Stop();

	std::function<void(Session*)> ConnectedEvent;
	std::function<void(Session*)> DisconnectedEvent;
private:
	std::shared_ptr<RedirectChannelFactory> m_RedirectChannelFactory;
	std::shared_ptr<RtspSession> _rtsp;
};

