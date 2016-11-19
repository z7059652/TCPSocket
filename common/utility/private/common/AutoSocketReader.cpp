#include "AutoSocketReader.h"
using namespace Titanium::TIRA;

bool ReadableSocket::IsSourceInError()
{
	return m_Socket->IsError();
}

void ReadableSocket::CancelRead()
{
	m_Socket->CancelRecv();
}

int ReadableSocket::ReadBytes(unsigned char* buffer, int bufLen)
{
	return m_Socket->Recv(buffer, bufLen);
}

ReadableSocket::ReadableSocket(std::shared_ptr<Socket> socket):
m_Socket(socket)
{

}