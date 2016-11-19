#pragma once

#include <atomic>
#include <string>
#include "SmartHandle.h"
#ifdef __GNUC__
#include <netinet/in.h>
#endif
namespace Titanium { namespace TIRA
{
	class InetAddress
	{
	public:
		InetAddress();
		explicit InetAddress(uint16_t port);

		InetAddress(const std::string& ip, uint16_t port);
		InetAddress(const struct sockaddr_in& addr);

		const struct sockaddr_in& getAddr() const;
		void setAddr(const struct sockaddr_in& addr);
		void setIpAdress(const std::string& ip);
		void setPort(uint16_t port);
		uint16_t getPort();
		std::string ToString() const;
		size_t HashCode() const;
		static bool GetInetAddressByHostname(const std::string hostname, std::string& ip);

	private:
		struct sockaddr_in m_addr;
		static const int MAX_HOST_IP_LENGTH = 16;
		static const int MAX_HOST_PORT_LENGTH = 22;
		static const int IPV4_ADDRESS_LENGTH = 16;
	};

	class SocketFuncs
	{
	private:
		SocketFuncs();
	public:
		static void Startup();
		static void Bind(SOCKET sockfd, const InetAddress& addr);
		static void ConnectTo(SOCKET sockfd, const InetAddress& addr);
		static void Shutdown(SOCKET sockfd);
		static SOCKET Accept(SOCKET sockfd);
		static void Listen(SOCKET sockfd, int connections);

		static bool SelectRead(SOCKET sockfd, int timeout);
		static bool SelectWrite(SOCKET sockfd, int timeout);
		static bool Select(SOCKET maxsock, fd_set* pWrite, fd_set* pRead, int timeout);

		static unsigned int GetBindedPort(SOCKET sockfd);

		static void SetSocketOption(SOCKET sockfd, int level, int optName, int optVal);
		static void SetSocketOption(SOCKET sockfd, int level, int optName, const char* optVal, int optLen);
		static void GetSocketOption(SOCKET sockfd, int level, int optName, char* optVal, socklen_t* optLen);
		static void GetSocketOption(SOCKET sockfd, int level, int optName, socklen_t* optVal);

	private:
		int temp;
	};

	class Socket
	{
	protected:
		Socket(SmartHandle<SOCKET> sockfd);
		virtual ~Socket();
	public:
		virtual void SetSocketOption(int level, int optName, socklen_t optVal);
		virtual void SetSocketOption(int level, int optName, const char* optVal, socklen_t optLen);
		virtual void GetSocketOption(int level, int optName, char* optVal, socklen_t* optLen);
		virtual void GetSocketOption(int level, int optName, socklen_t* optVal);

		virtual int Send(const unsigned char* buffer, int bufLen);
		virtual int SendTo(const InetAddress& pDest, const char* data, int len);
		virtual void CancelSend();

		virtual int Recv(unsigned char* buffer, int bufLen);
		virtual int ReceiveFrom(unsigned char* data, int len, InetAddress* pSrc);
		virtual void CancelRecv();

		virtual void Shutdown();
		virtual bool IsAlive();
		virtual bool IsError();
		virtual void GetPeerAddress(InetAddress* address);
	protected:
		void WaitForReadReady();
		void WaitForWriteReady();
		int CheckSocketReturn(int result);

		SmartHandle<SOCKET> _sockfd;
	private:
		std::atomic<bool>		m_IsAlive;
		std::atomic<bool>		m_IsError;
		std::atomic<bool>		m_IsCancelRead;
		std::atomic<bool>		m_IsCancelWrite;
		static const int WAITING_TIME = 500;
	};
}
}
