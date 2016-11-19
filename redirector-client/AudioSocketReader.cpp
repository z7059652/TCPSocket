#include "AudioSocketReader.h"


AudioSocketReader::AudioSocketReader(std::shared_ptr<Titanium::TIRA::SocketTcp> socket)
	:AsyncBase("AudioSocketReader")
	, m_socket(socket)
{
}


AudioSocketReader::~AudioSocketReader()
{
}

void AudioSocketReader::Start()
{
	StartAsyncBase();
}

void AudioSocketReader::Stop()
{
	m_socket->CancelRecv();
	StopAsyncBase();
}

void AudioSocketReader::Workloop()
{
	while (CanLoopContinue())
	{
		unsigned char * buf = (unsigned char *)malloc(1000);
		unsigned char* bufpointer = buf;
		unsigned int size = 0;
		int len = 0;
		int lentemp = 0;
		try
		{
			lentemp = m_socket->Recv((unsigned char*)&size, sizeof(unsigned int));
		}
		catch (...) { return; }
		while (1)
		{
			try
			{
				lentemp = m_socket->Recv(bufpointer, min(size, size - len));
			}
			catch (...) { return; }
			len += lentemp;
			bufpointer += lentemp;
			if (len == size)
			{
				OnDataReceivedEvent(buf, size);
				break;
			}
		}
		free(buf);
	}
}

void AudioSocketReader::Cancel()
{
	m_socket->CancelRecv();
}
