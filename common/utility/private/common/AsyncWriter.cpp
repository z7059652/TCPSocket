#include "AsyncWriter.h"
using namespace Titanium::TIRA;

AsyncWriter::AsyncWriter(std::shared_ptr<WritableSource> source, unsigned int bufferSize, unsigned int maxPkgSize, unsigned int minPkgSize)
	: AsyncBase("AsyncWriter")
	, m_Source(source)
	, m_MaxSize(maxPkgSize)
	, m_MinSize(minPkgSize)
	, m_Buffer(bufferSize)
{
	m_TempBuffer.reset(new unsigned char[m_MaxSize]);
	StartAsyncBase();
}

void AsyncWriter::UnBlockWorkingThread()
{
	Cancel();
}

void AsyncWriter::Workloop()
{
	while (CanLoopContinue())
	{
		unsigned int readCount = m_Buffer.ReadAtless(
			m_TempBuffer.get(),
			m_MaxSize,
			m_MinSize);
		if (readCount > 0)
		{
			m_Source->WriteBytes(m_TempBuffer.get(), readCount);
		}
	}
}

bool AsyncWriter::IsSourceInError()
{
	return m_Source->IsSourceInError() || IsAsyncBaseInError();
}

void AsyncWriter::Close()
{
	StopAsyncBase();
}

void AsyncWriter::Cancel()
{
	m_Source->Cancel();
	m_Buffer.CancelRead();
	m_Buffer.CancelWrite();
	m_Buffer.CancelWait();
}

void AsyncWriter::WriteBytes(unsigned const char* buffer, int len)
{
	ENSURE(IsAsyncBaseRunning())("Current Writer is not running, it may have either been Closed or encounted error!")(m_status);
	m_Buffer.WriteTillFinish(buffer, len);
}

void AsyncWriter::WaitTillSourceIdeal()
{
	m_Buffer.WaitTillBufferEmpty();
	m_Source->WaitTillSourceIdeal();
}

AsyncWriter::~AsyncWriter()
{
	MuteAllExceptions([&]() { Close(); });
}
