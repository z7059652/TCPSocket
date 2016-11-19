#include "HIDSocketReader.h"


HIDSocketReader::HIDSocketReader(std::shared_ptr<Titanium::TIRA::SocketTcp> socket)
:AsyncBase("HIDSocketReader")
,m_socket(socket)
{
}


HIDSocketReader::~HIDSocketReader()
{
}

void HIDSocketReader::Start()
{
	StartAsyncBase();
}

void HIDSocketReader::Stop()
{
	StopAsyncBase();
}

void HIDSocketReader::Workloop()
{
	unsigned char buf[sizeof(HIDDATASTRUCT)];
	unsigned char* bufpointer = buf;
	int len = 0;
	while (CanLoopContinue())
	{
		int lentemp = m_socket->Recv(bufpointer, min(sizeof(HIDDATASTRUCT), sizeof(HIDDATASTRUCT)-len));
		len += lentemp;
		bufpointer += lentemp;
		if (len == sizeof(HIDDATASTRUCT))
		{
			HIDDATASTRUCT* phiddata = (HIDDATASTRUCT*)buf;
			if (OnDataReceivedEvent)
			{
				OnDataReceivedEvent(phiddata);
			}
			len = 0;
			memset(buf, 0, sizeof(HIDDATASTRUCT));
			bufpointer = buf;
		}
	}
}

void HIDSocketReader::Cancel()
{
	m_socket->CancelRecv();
}
