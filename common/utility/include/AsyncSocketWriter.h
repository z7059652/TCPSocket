#pragma once
#include "AsyncWriter.h"
#include "Socket.h"
#include <memory>
#include <atomic>

#define ASYNC_SOCKET_WRITER_BUFFER_SIZE 1024 * 1024 * 1 // 1M
#define ASYNC_SOCKET_WRITER_MAX_WRITE_SIZE 1024 * 32 // 32K, half of max TCP buffer size;
#define ASYNC_SOCKET_MIN_WRITE_SIZE 1

namespace Titanium { namespace TIRA
{
	class WritableSocket :public WritableSource
	{
	public:
		WritableSocket(std::shared_ptr<Socket> socket);
	public:
		virtual void WaitTillSourceIdeal();
		virtual void WriteBytes(unsigned const char* buffer, int bufLen);
		virtual void Cancel();
		virtual bool IsSourceInError();
	private:
		std::shared_ptr<Socket> m_Socket;
	};

	class AsyncSocketWriter : public AsyncWriter
	{
	public:
		AsyncSocketWriter(std::shared_ptr<Socket> socket, unsigned int maxPkgSize = ASYNC_SOCKET_WRITER_MAX_WRITE_SIZE, unsigned int minPkgSize = ASYNC_SOCKET_MIN_WRITE_SIZE);
	};
}
}
