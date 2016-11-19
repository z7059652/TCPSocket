#pragma once
#include "AsyncBase.h"
#include "ReadableSource.h"
#include "SignalConnection.h"
#define AUTOREADER_BUF_SIZE 1024*6

namespace Titanium { namespace TIRA
{
	class AutoReader : public AsyncBase
	{
	public:
		typedef std::function<void(const unsigned char*, unsigned int)> OnDataReadedAction;
		AutoReader(std::shared_ptr<ReadableSource> source);
		virtual ~AutoReader();
		SignalConnection RegisterDataReadedEvent(OnDataReadedAction action);
		virtual bool IsSourceInError();
		void Close();
	protected:
		virtual void Workloop();
		virtual void UnBlockWorkingThread();
		virtual void OnDataReaded(const unsigned char* pData, unsigned int len);
	private:
		std::shared_ptr<ReadableSource> m_Source;
		Signal<void(const unsigned char*, unsigned int)> m_SigDataReaded;
		std::unique_ptr<unsigned char[]> m_InternalBuffer;
	};
}
}

