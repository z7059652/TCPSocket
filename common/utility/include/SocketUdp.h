#pragma once
#include "Socket.h"

namespace Titanium { namespace TIRA
{
	class SocketUdp : public Socket
	{
	public:
		SocketUdp(void);
		void Bind(const InetAddress& addr);
		virtual int Send(const unsigned char* buffer, int bufLen);
		virtual int Recv(unsigned char* buffer, int bufLen);
		void SetSenderAddr(const struct sockaddr_in& addr, uint16_t port);
		const InetAddress& GetRecvAddr() const;
		void SetSockOptBroadcast(bool yes);
		void SetSockOptJoinMulticastGroup(const ip_mreq & group);
		void SetSockOptWithdrawMulticastGroup(const ip_mreq & group);
		void SetSockOptReuseAddress(bool yes);
		void SetSockOptMulticastLoop(bool yes);
		void SetSockOptMulticastTTL(int val);
	private:
		InetAddress m_SendAddress;
		InetAddress m_RecvAddress;
	};
}
}
