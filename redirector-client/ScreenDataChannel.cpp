#include "ScreenDataChannel.h"

ScreenDataChannel::ScreenDataChannel(std::shared_ptr<Titanium::TIRA::SocketTcp> socket)
:AsyncBase("ScreenDataChannel")
, m_socket(socket)
{
}


ScreenDataChannel::~ScreenDataChannel()
{
}

void ScreenDataChannel::Start()
{
	StartAsyncBase();
}

void ScreenDataChannel::Stop()
{
	StopAsyncBase();
}

void ScreenDataChannel::Workloop()
{
	unsigned char buf[sizeof(RDPGFX_FRAME_ACKNOWLEDGE_PDU)];
	unsigned char* bufpointer = buf;
	int len = 0;
	while (CanLoopContinue())
	{
		int lentemp = m_socket->Recv(bufpointer, min(sizeof(RDPGFX_FRAME_ACKNOWLEDGE_PDU), sizeof(RDPGFX_FRAME_ACKNOWLEDGE_PDU)-len));
		len += lentemp;
		bufpointer += lentemp;
		if (len == sizeof(RDPGFX_FRAME_ACKNOWLEDGE_PDU))
		{
			RDPGFX_FRAME_ACKNOWLEDGE_PDU* pScreedata = (RDPGFX_FRAME_ACKNOWLEDGE_PDU*)buf;
			if (OnDataReceivedEvent)
			{
				OnDataReceivedEvent(pScreedata);
			}
			len = 0;
			memset(buf, 0, sizeof(RDPGFX_FRAME_ACKNOWLEDGE_PDU));
			bufpointer = buf;
		}
	}
}

void ScreenDataChannel::Cancel()
{
	m_socket->CancelRecv();
}

void ScreenDataChannel::Send(void *data, unsigned int size)
{
	m_socket->Send((const unsigned char*)&size, 4);
	m_socket->Send((const unsigned char*)data, size);
}
