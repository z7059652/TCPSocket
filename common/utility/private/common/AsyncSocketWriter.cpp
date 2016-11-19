#include "AsyncSocketWriter.h"
using namespace Titanium::TIRA;

bool WritableSocket::IsSourceInError()
{
	return m_Socket->IsError();
}

void WritableSocket::Cancel()
{
	m_Socket->CancelSend();
}

void WritableSocket::WriteBytes(unsigned const char* buffer, int bufLen)
{
	m_Socket->Send(buffer, bufLen);
}

void WritableSocket::WaitTillSourceIdeal()
{

}

WritableSocket::WritableSocket(std::shared_ptr<Socket> socket)
	: m_Socket(socket)
{

}

AsyncSocketWriter::AsyncSocketWriter(std::shared_ptr<Socket> socket, unsigned int maxPkgSize /*= ASYNC_SOCKET_WRITER_MAX_WRITE_SIZE*/, unsigned int minPkgSize /*= ASYNC_SOCKET_MIN_WRITE_SIZE*/)
	: AsyncWriter(std::make_shared<WritableSocket>(socket), ASYNC_SOCKET_WRITER_BUFFER_SIZE, maxPkgSize, minPkgSize)
{
}