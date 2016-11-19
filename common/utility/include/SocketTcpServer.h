#pragma once
#include <atomic>
#include "SocketTcp.h"

namespace Titanium { namespace TIRA
{
	class SocketTcpServer
	{
	public:
		SocketTcpServer(int port, int connections = SOMAXCONN);
		~SocketTcpServer();

		void StartListening();
		void StopListening();
		unsigned GetListeningPort();
		std::shared_ptr<SocketTcp> WaitForConnection();
	private:
		SmartHandle<SOCKET>  m_Sockfd;
		unsigned int m_Connections;
		std::atomic<bool> m_AlreadyShutdown;
	};
}
}
