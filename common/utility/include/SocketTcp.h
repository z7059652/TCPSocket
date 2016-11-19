#pragma once
#include <memory>
#include "Socket.h"

namespace Titanium { namespace TIRA
{
	class SocketTcp : public Socket
	{
		friend class SocketTcpServer;
	public:
		static std::shared_ptr<SocketTcp> ConnectTo(const InetAddress& addr);
		int SetRecvBufferSize(socklen_t buffsz = SOCKET_TCP_MAX_BUF_SIZE);
		int SetSendBufferSize(socklen_t buffsz = SOCKET_TCP_MAX_BUF_SIZE);
		void SetTcpNoDelay(bool yes);
	private:
		SocketTcp(SmartHandle<SOCKET> sockfd);
		static const unsigned int SOCKET_TCP_STANDARD_BUF_SIZE = (8 * 1024);
		static const unsigned int SOCKET_TCP_MAX_BUF_SIZE = (64 * 1024 - 1);
	};
}
}

