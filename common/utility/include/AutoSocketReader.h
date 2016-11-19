#pragma once
#include "AutoReader.h"
#include "Socket.h"

namespace Titanium { namespace TIRA
{
	class ReadableSocket :public ReadableSource
	{
	public:
		ReadableSocket(std::shared_ptr<Socket> socket);
		virtual int ReadBytes(unsigned char* buffer, int bufLen);
		virtual void CancelRead();
		virtual bool IsSourceInError();
	private:
		std::shared_ptr<Socket> m_Socket;
	};
}
}
