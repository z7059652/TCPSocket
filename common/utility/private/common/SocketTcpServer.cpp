#include "SocketTcpServer.h"
using namespace Titanium::TIRA;

std::shared_ptr<SocketTcp> SocketTcpServer::WaitForConnection()
{
	SmartHandle<SOCKET> sockfd(SocketFuncs::Accept(m_Sockfd.get()));
	std::shared_ptr<SocketTcp> tcpSocket;
	tcpSocket.reset(new SocketTcp(sockfd));
	return tcpSocket;
}

unsigned SocketTcpServer::GetListeningPort()
{
	return SocketFuncs::GetBindedPort(m_Sockfd.get());
}

void SocketTcpServer::StopListening()
{
	bool alreadyShutdown = false;
	m_AlreadyShutdown.compare_exchange_strong(alreadyShutdown, true);
	if (alreadyShutdown)
		return;

	SocketFuncs::Shutdown(m_Sockfd.get());
}

void SocketTcpServer::StartListening()
{
	SocketFuncs::Listen(m_Sockfd.get(), m_Connections);
}

SocketTcpServer::~SocketTcpServer()
{
	StopListening();
}

SocketTcpServer::SocketTcpServer(int port, int connections)
	: m_Sockfd(SmartHandle<SOCKET>(socket(AF_INET, SOCK_STREAM, 0)))
{
	InetAddress addr(port);
	m_Connections = connections;
#ifdef __GNUC__
	int optVal = 1;
	ENSURE(SOCKET_ERROR != setsockopt(m_Sockfd.get(), SOL_SOCKET, SO_REUSEADDR, (char*)&optVal, sizeof(optVal)))(WSAGetLastError());
#endif
	SocketFuncs::Bind(m_Sockfd.get(), addr);
}