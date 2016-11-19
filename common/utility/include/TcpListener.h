#pragma once
#include <functional>
#include "SocketTcpServer.h"
#include "AsyncBase.h"
#include "SignalConnection.h"

namespace Titanium
{
	class TcpListener : TIRA::AsyncBase
	{
		typedef std::function<void(std::shared_ptr<TIRA::SocketTcp>)> OnConnectionAcceptedAction;
	public:
		TcpListener(unsigned int port);
		~TcpListener(void);
	public:
		TIRA::SignalConnection RegisterConnectionAcceptedEvent(OnConnectionAcceptedAction action);

		virtual void Start();
		virtual void Stop();
	protected:
		virtual void Workloop();
		virtual void UnBlockWorkingThread();
	private:
		std::shared_ptr<TIRA::SocketTcpServer> m_pServer;
		TIRA::Signal<void(std::shared_ptr<TIRA::SocketTcp>)> m_SigConnected;

		unsigned int m_Port;
	};
}

