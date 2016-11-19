#include "AutoReader.h"
#include "error_handling_utility.h"
using namespace Titanium::TIRA;

void AutoReader::OnDataReaded(const unsigned char* pData, unsigned int len)
{
	m_SigDataReaded(pData, len);
}

void AutoReader::UnBlockWorkingThread()
{
	m_Source->CancelRead();
}

void AutoReader::Workloop()
{
	while (CanLoopContinue())
	{
		int readCount = m_Source->ReadBytes(m_InternalBuffer.get(), AUTOREADER_BUF_SIZE);
		if (readCount > 0)
		{
			OnDataReaded(m_InternalBuffer.get(), readCount);
		}
	}
}

void AutoReader::Close()
{
	StopAsyncBase();
}

bool AutoReader::IsSourceInError()
{
	return IsAsyncBaseInError();
}

SignalConnection AutoReader::RegisterDataReadedEvent(OnDataReadedAction action)
{
	return std::move(m_SigDataReaded.connect(action));
}

AutoReader::~AutoReader()
{
	MuteAllExceptions([&]() { Close(); });
}

AutoReader::AutoReader(std::shared_ptr<ReadableSource> source)
	: AsyncBase("AutoReader")
	, m_Source(source)
{
	m_InternalBuffer.reset(new unsigned char[AUTOREADER_BUF_SIZE]);
	StartAsyncBase();
}