#pragma once
#include "WaitingBuffer.h"
#include "AsyncBase.h"
#include "WritableSource.h"

namespace Titanium { namespace TIRA
{
	class AsyncWriter :AsyncBase, public WritableSource
	{
	public:
		AsyncWriter(std::shared_ptr<WritableSource> source, unsigned int bufferSize, unsigned int maxPkgSize, unsigned int minPkgSize);
		virtual ~AsyncWriter();
		virtual void WaitTillSourceIdeal();
		virtual void WriteBytes(unsigned const char* buffer, int len);
		virtual void Cancel();
		void Close();
		virtual bool IsSourceInError();

	protected:
		virtual void Workloop();
		virtual void UnBlockWorkingThread();

	private:
		WaitingBuffer<unsigned char>	m_Buffer;
		std::shared_ptr<WritableSource> m_Source;
		std::unique_ptr<unsigned char[]>	m_TempBuffer; //1K
		unsigned int m_MaxSize;
		unsigned int m_MinSize;
	};
}
}

