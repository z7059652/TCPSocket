#include "SocketTcp.h"
#ifdef __GNUC__
#include <linux/tcp.h>
#endif
using namespace Titanium::TIRA;

SocketTcp::SocketTcp(SmartHandle<SOCKET> sockfd)
	: Socket(sockfd)
{
}

std::shared_ptr<SocketTcp> SocketTcp::ConnectTo(const InetAddress& addr)
{
	SmartHandle<SOCKET> sockfd(socket(AF_INET, SOCK_STREAM, 0));
	SocketFuncs::ConnectTo(sockfd.get(), addr);
	std::shared_ptr<SocketTcp> tcpSock;
	tcpSock.reset(new SocketTcp(sockfd));
	return tcpSock;
}

void SocketTcp::SetTcpNoDelay(bool yes)
{
	int opt = yes ? 1 : 0;

	SetSocketOption(IPPROTO_TCP, TCP_NODELAY, opt);
}

int SocketTcp::SetSendBufferSize(socklen_t buffsz /*= SOCKET_TCP_MAX_BUF_SIZE*/)
{
	SetSocketOption(SOL_SOCKET, SO_SNDBUF, buffsz);
	GetSocketOption(SOL_SOCKET, SO_SNDBUF, &buffsz);

	return buffsz;
}

int SocketTcp::SetRecvBufferSize(socklen_t buffsz /*= SOCKET_TCP_MAX_BUF_SIZE*/)
{
	SetSocketOption(SOL_SOCKET, SO_RCVBUF, buffsz);
	GetSocketOption(SOL_SOCKET, SO_RCVBUF, &buffsz);

	return buffsz;
}