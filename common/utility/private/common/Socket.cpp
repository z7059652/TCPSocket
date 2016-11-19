#include "Socket.h"
#include "error_handling_utility.h"
#pragma comment(lib, "ws2_32")
#ifdef __GNUC__
#define MAKEWORD(a,b) 0
#define WSAStartup(a,b) 0
#define WSAECONNRESET 0
#define WSAECONNABORTED 0
#define WSAENOTCONN 0
#define WSAEINPROGRESS 0
#define WSADATA int
#define ioctlsocket ioctl
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif
using namespace Titanium::TIRA;

bool InetAddress::GetInetAddressByHostname(const std::string hostname, std::string& ip)
{
	struct addrinfo	hints, *res;
	int	error = 0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	error = getaddrinfo(hostname.c_str(), "", &hints, &res);
	if (error)
	{
		// unable to resolve the name
		return false;
	}

	// for now, we just use the first ip address, maybe use ip address list in the future.


	char ipaddr[30];
	inet_ntop(AF_INET, &((struct sockaddr_in*)res->ai_addr)->sin_addr, ipaddr, sizeof(ipaddr));
	ip = ipaddr;

	return true;
}

size_t InetAddress::HashCode() const
{
	size_t h = 0;
	std::string hostPort = this->ToString();
	std::string::const_iterator p, p_end;
	for (p = hostPort.begin(), p_end = hostPort.end(); p != p_end; ++p)
	{
		h = 31 * h + (*p);
	}
	return h;
}

std::string InetAddress::ToString() const
{
	char hostPort[MAX_HOST_PORT_LENGTH], host[MAX_HOST_IP_LENGTH]; // TODO:change number to const.
	uint16_t port;

	inet_ntop(AF_INET, (void*)&m_addr.sin_addr, host, sizeof(host));
	port = ntohs(m_addr.sin_port);
	sprintf(hostPort, "%s:%u", host, port);
	return hostPort;
}

uint16_t InetAddress::getPort()
{
	return ntohs(m_addr.sin_port);
}

void InetAddress::setPort(uint16_t port)
{
	m_addr.sin_port = htons(port);
}

void InetAddress::setIpAdress(const std::string& ip)
{
	if (inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr) <= 0)
	{
		throw ExceptionWithString("ip address is not valid.");
	}
}

void InetAddress::setAddr(const struct sockaddr_in& addr)
{
	m_addr = addr;
}

const struct sockaddr_in& InetAddress::getAddr() const
{
	return m_addr;
}

InetAddress::InetAddress(const struct sockaddr_in& addr):m_addr(addr)
{

}

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
	memset(&m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, ip.c_str(), &m_addr.sin_addr) <= 0)
		throw ExceptionWithString("ip address is not valid.");
}

InetAddress::InetAddress(uint16_t port)
{
	memset(&m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);
	m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
}

InetAddress::InetAddress()
{
	memset(&m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(0);
	m_addr.sin_addr.s_addr = htonl(INADDR_ANY);
}

void SocketFuncs::GetSocketOption(SOCKET sockfd, int level, int optName, socklen_t* optVal)
{
	socklen_t optLen = sizeof(int);
	ENSURE(SOCKET_ERROR != getsockopt(sockfd, level, optName, (char*)optVal, &optLen))(WSAGetLastError());
}

void SocketFuncs::GetSocketOption(SOCKET sockfd, int level, int optName, char* optVal, socklen_t* optLen)
{
	ENSURE(SOCKET_ERROR != getsockopt(sockfd, level, optName, optVal, optLen))(WSAGetLastError());
}

void SocketFuncs::SetSocketOption(SOCKET sockfd, int level, int optName, const char* optVal, int optLen)
{
	ENSURE(SOCKET_ERROR != setsockopt(sockfd, level, optName, optVal, optLen))(WSAGetLastError());
}

void SocketFuncs::SetSocketOption(SOCKET sockfd, int level, int optName, int optVal)
{
	ENSURE(SOCKET_ERROR != setsockopt(sockfd, level, optName, (char*)&optVal, sizeof(optVal)))(WSAGetLastError());
}

unsigned int SocketFuncs::GetBindedPort(SOCKET sockfd)
{
	struct sockaddr_in sin;
	socklen_t addrlen = sizeof(sin);
	if (getsockname(sockfd, (struct sockaddr *)&sin, &addrlen) == 0 &&
		sin.sin_family == AF_INET &&
		addrlen == sizeof(sin))
	{
		return ntohs(sin.sin_port);
	}
	return 0;
}

bool SocketFuncs::Select(SOCKET maxsock, fd_set* pWrite, fd_set* pRead, int timeout)
{
	struct timeval tTimeout;
	tTimeout.tv_sec = timeout / 1000;
	tTimeout.tv_usec = timeout % 1000 * 1000;

	int result = select((int)maxsock + 1, pRead, pWrite, NULL, &tTimeout);
	if (result == SOCKET_ERROR)
	{
		int error = WSAGetLastError();

		ENSURE(WSAECONNRESET == error
			|| WSAECONNABORTED == error
			|| WSAENOTCONN == error
			|| WSAEINPROGRESS == error)(error);
		return false;
	}
	return result > 0;
}

bool SocketFuncs::SelectWrite(SOCKET sockfd, int timeout)
{
	fd_set writeFds;
	FD_ZERO(&writeFds);
	FD_SET(sockfd, &writeFds);
	return Select(sockfd, &writeFds, nullptr, timeout);
}

bool SocketFuncs::SelectRead(SOCKET sockfd, int timeout)
{
	fd_set readFds;
	FD_ZERO(&readFds);
	FD_SET(sockfd, &readFds);

	return Select(sockfd, nullptr, &readFds, timeout);
}

void SocketFuncs::Listen(SOCKET sockfd, int connections)
{
	ENSURE(SOCKET_ERROR != listen(sockfd, connections))(WSAGetLastError());
}

SOCKET SocketFuncs::Accept(SOCKET sockfd)
{
	while (true)
	{
		SOCKET rt = accept(sockfd, nullptr, 0);
		if (INVALID_SOCKET == rt)
		{
			int error = WSAGetLastError();
			if (error != WSAECONNRESET)
				throw ExceptionWithString("SocketFuncs::Accept aborted.");
			continue;
		}
		return rt;
	}
}

void SocketFuncs::Shutdown(SOCKET sockfd)
{
	ON_SCOPE_EXIT([&]()
	{
		closesocket(sockfd);
	});

	int result = shutdown(sockfd, /* SD_BOTH*/0x02);

	if (result == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		ENSURE(WSAECONNRESET == error
			|| WSAECONNABORTED == error
			|| WSAENOTCONN == error
			|| WSAEINPROGRESS == error)(error);
	}
}

void SocketFuncs::ConnectTo(SOCKET sockfd, const InetAddress& addr)
{
	const struct sockaddr_in& bindAddr = addr.getAddr();
	ENSURE(SOCKET_ERROR != connect(sockfd, (const sockaddr *)&bindAddr, sizeof(bindAddr)))(WSAGetLastError());
}

void SocketFuncs::Bind(SOCKET sockfd, const InetAddress& addr)
{
	const struct sockaddr_in& bindAddr = addr.getAddr();
	ENSURE(SOCKET_ERROR != bind(sockfd, (const sockaddr *)&bindAddr, sizeof(bindAddr)))(WSAGetLastError());
}

void SocketFuncs::Startup()
{
	WSADATA wsaData;
	ENSURE(WSAStartup(MAKEWORD(2, 0), &wsaData) != SOCKET_ERROR)(WSAGetLastError());
}

SocketFuncs::SocketFuncs()
{

}

Socket::Socket(SmartHandle<SOCKET> sockfd):
_sockfd(sockfd)
{
	unsigned long nIoctlOpt = 1;
	ioctlsocket(_sockfd.get(), FIONBIO, &nIoctlOpt); 	// set socket to non-blocking mode.
	m_IsAlive = true;
	m_IsCancelRead = false;
	m_IsCancelWrite = false;
}

int Socket::CheckSocketReturn(int result)
{
	if (SOCKET_ERROR == result)
	{
		m_IsError = true;
#ifdef __GNUC__
		auto error = errno;
#else
		auto error = (WSAGetLastError());
#endif
		std::ostringstream os;
		os << "Socket encounter error: " << error << std::endl;
		throw ExceptionWithString(os.str());
	}
	return result;
}

void Socket::WaitForWriteReady()
{
	ON_SCOPE_EXIT([&]()
	{
		m_IsCancelWrite = false;
	});
	while (true)
	{
		if (m_IsCancelWrite)
			throw ExceptionWithString("Writing is canceled!");

		if (SocketFuncs::SelectWrite(_sockfd.get(), WAITING_TIME))
			return;
	}
}

void Socket::WaitForReadReady()
{
	ON_SCOPE_EXIT([&]()
	{
		m_IsCancelRead = false;
	});
	while (true)
	{
		if (true == m_IsCancelRead)
			throw ExceptionWithString("Socket: Reading is canceled!");

		if (SocketFuncs::SelectRead(_sockfd.get(), WAITING_TIME))
			return;
	}
}

void Socket::GetPeerAddress(InetAddress* address)
{
	sockaddr add;
	socklen_t len = sizeof(add);
	::getpeername(_sockfd.get(), &add, &len);
	address->setAddr(*((sockaddr_in*)&add));
}

bool Socket::IsError()
{
	return m_IsError;
}

bool Socket::IsAlive()
{
	return m_IsAlive;
}

void Socket::Shutdown()
{
	bool isAlive = true;
	m_IsAlive.compare_exchange_strong(isAlive, false);
	if (isAlive)
	{
		MuteAllExceptions([&]()
		{
			CancelSend();
			CancelRecv();
			SocketFuncs::Shutdown(_sockfd.get());
		}
		, "Shutdown Socket!");
	}
}

void Socket::CancelRecv()
{
	m_IsCancelRead = true;
}

int Socket::ReceiveFrom(unsigned char* data, int len, InetAddress* pSrc)
{
	if (!m_IsAlive)
		throw ExceptionWithString("Call on a dead socket!");
	WaitForReadReady();

	struct sockaddr_in srcAddr;
	memset(&srcAddr, 0, sizeof(srcAddr));
	socklen_t addrLen = sizeof(srcAddr);

	int recvCount = ::recvfrom(_sockfd.get(), (char*)data, len, 0, (sockaddr*)&srcAddr, &addrLen);
	if (recvCount == 0)
		throw ExceptionWithString("Socket has been closed gracefully!");
	if (recvCount > 0)
	{
		pSrc->setAddr(srcAddr);
	}
	return CheckSocketReturn(recvCount);
}

int Socket::Recv(unsigned char* buffer, int bufLen)
{
	if (!m_IsAlive)
		throw ExceptionWithString("Call on a dead socket!");
	WaitForReadReady();
	int recvCount = recv(_sockfd.get(), (char*)buffer, bufLen, 0);
	if (recvCount == 0)
		throw ExceptionWithString("Socket has been closed gracefully!");
	return CheckSocketReturn(recvCount);
}

void Socket::CancelSend()
{
	m_IsCancelWrite = true;
}

int Socket::SendTo(const InetAddress& pDest, const char* data, int len)
{
	if (!m_IsAlive)
		throw ExceptionWithString("Call on a dead socket!");

	WaitForWriteReady();

	const struct sockaddr_in& dsetAddr = pDest.getAddr();
	int sentCount = ::sendto(_sockfd.get(), (const char*)data, len, 0, (const sockaddr *)&dsetAddr, sizeof(dsetAddr));
	return CheckSocketReturn(sentCount);
}

int Socket::Send(const unsigned char* buffer, int bufLen)
{
	if (!m_IsAlive)
		throw ExceptionWithString("Call on a dead socket!");
	WaitForWriteReady();
	int sentCount = send(_sockfd.get(), (const char*)buffer, bufLen, 0);
	return CheckSocketReturn(sentCount);
}

void Socket::GetSocketOption(int level, int optName, socklen_t* optVal)
{
	SocketFuncs::GetSocketOption(_sockfd.get(), level, optName, optVal);
}

void Socket::GetSocketOption(int level, int optName, char* optVal, socklen_t* optLen)
{
	SocketFuncs::GetSocketOption(_sockfd.get(), level, optName, optVal, optLen);
}

void Socket::SetSocketOption(int level, int optName, const char* optVal, socklen_t optLen)
{
	SocketFuncs::SetSocketOption(_sockfd.get(), level, optName, optVal, optLen);
}

void Socket::SetSocketOption(int level, int optName, socklen_t optVal)
{
	SocketFuncs::SetSocketOption(_sockfd.get(), level, optName, optVal);
}

Socket::~Socket()
{
	Shutdown();
}