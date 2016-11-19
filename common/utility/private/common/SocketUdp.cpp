#include "SocketUdp.h"
#include "error_handling_utility.h"
using namespace Titanium::TIRA;

void SocketUdp::SetSockOptMulticastTTL(int val)
{
	SetSocketOption(IPPROTO_IP, IP_MULTICAST_TTL, val);
}

void SocketUdp::SetSockOptMulticastLoop(bool yes)
{
	int opt = yes ? 1 : 0;
	SetSocketOption(IPPROTO_IP, IP_MULTICAST_LOOP, opt);
}

void SocketUdp::SetSockOptReuseAddress(bool yes)
{
	int opt = yes ? 1 : 0;
	SetSocketOption(SOL_SOCKET, SO_REUSEADDR, opt);
}

void SocketUdp::SetSockOptWithdrawMulticastGroup(const ip_mreq & group)
{
	SetSocketOption(IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)(&group), sizeof(ip_mreq));
}

void SocketUdp::SetSockOptJoinMulticastGroup(const ip_mreq & group)
{
	SetSocketOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)(&group), sizeof(ip_mreq));
}

void SocketUdp::SetSockOptBroadcast(bool yes)
{
	int opt = yes ? 1 : 0;
	SetSocketOption(SOL_SOCKET, SO_BROADCAST, opt);
}

const InetAddress& SocketUdp::GetRecvAddr() const
{
	return m_RecvAddress;
}

void SocketUdp::SetSenderAddr(const struct sockaddr_in& addr, uint16_t port)
{
	m_SendAddress.setAddr(addr);
	m_SendAddress.setPort(port);
}

int SocketUdp::Recv(unsigned char* buffer, int bufLen)
{
	return ReceiveFrom(buffer, bufLen, &m_RecvAddress);
}

int SocketUdp::Send(const unsigned char* buffer, int bufLen)
{
	return SendTo(m_SendAddress, (const char*)buffer, bufLen);
}

void SocketUdp::Bind(const InetAddress& addr)
{
	SocketFuncs::Bind(_sockfd.get(), addr);
}
SocketUdp::SocketUdp(void):
Socket(SmartHandle<SOCKET>(socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
{
}