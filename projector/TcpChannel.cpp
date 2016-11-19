#include "AutoReader.h"
#include "AsyncSocketWriter.h"
#include "AutoSocketReader.h"
#include "SocketTcp.h"
#include "TcpChannel.h"
using namespace Titanium;
using namespace Titanium::TIRA;

#define READING_BUF_SIZE 16
#define WRITING_BUF_SIZE 1316
#define random(x) (rand()%x)

TcpChannel::TcpChannel(std::shared_ptr<SocketTcp> socket)
	: AsyncBase("SimpleTcpChannel")
	, m_Writer(std::make_shared<AsyncSocketWriter>(socket))
	, m_Reader(std::make_shared<ReadableSocket>(socket))
{
	m_InternalBuffer.SetBufferSizeLowerBound(READING_BUF_SIZE);
	m_InternalWritingBuffer.SetBufferSizeLowerBound(WRITING_BUF_SIZE);
	SetChannelName("SimpleTcpChannel");
}

TcpChannel::~TcpChannel()
{
}

void TcpChannel::Send(const unsigned char* pData, unsigned int dataLen)
{
	//if (!IsAsyncBaseRunning())
		//throw ExceptionWithString("Channel is not started!");
	while (dataLen > 0)
	{
		unsigned int canWrite = MIN(dataLen, m_InternalWritingBuffer.GetSpaceLeave());
		m_InternalWritingBuffer.AppendFrom(pData, canWrite);
		pData += canWrite;
		dataLen -= canWrite;
		//if (0 == m_InternalWritingBuffer.GetSpaceLeave())
		//{
			Flush();
		//}
	}
}

void TcpChannel::CancelSend()
{
	m_Writer->Cancel();
}

void TcpChannel::Flush()
{
//	ENSURE(IsAsyncBaseRunning())("Flush on a dead channel.");
	if (m_InternalWritingBuffer.GetDataLength() > 0)
	{
		m_Writer->WaitTillSourceIdeal();

		m_Writer->WriteBytes(m_InternalWritingBuffer.GetPtrForReading(), m_InternalWritingBuffer.GetDataLength());
		m_InternalWritingBuffer.Clear();
	}
}
SignalConnection TcpChannel::RegisterDataReceivedEvent(OnDataReceivedAction action)
{
	return m_SignalDataReceived.connect(action);
}
void TcpChannel::OnDataReceived(const unsigned char* pData, unsigned int len)
{
	m_SignalDataReceived(pData, len);
}
void TcpChannel::Workloop()
{
	while (CanLoopContinue())
	{
		int readCount = m_Reader->ReadBytes(m_InternalBuffer.GetPtrForWriting(), m_InternalBuffer.GetBufferSize());
		if (readCount > 0)
		{
			OnDataReceived(m_InternalBuffer.GetPtrForReading(), readCount);
		}
	}
}
void TcpChannel::UnBlockWorkingThread()
{
	m_Reader->CancelRead();
	m_Writer->Cancel();
}
bool TcpChannel::GetChannelState(ChannelState& curGetChannelState)
{
	return false;
}
void TcpChannel::Start()
{
	StartAsyncBase();
}
void TcpChannel::Stop()
{
	StopAsyncBase();
}
bool TcpChannel::IsInError()
{
	return IsAsyncBaseInError();
}
void TcpChannel::SetChannelName(const std::string& SetName)
{
	m_ChannelName = SetName;
}
std::string TcpChannel::GetChannelName()
{
	return m_ChannelName;
}