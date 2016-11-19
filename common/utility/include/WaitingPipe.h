#pragma once
#include "WaitingQueue.h"

namespace Titanium { namespace TIRA
{
	template<typename T>
	class WaitingPipe
	{
	public:
		void PushEmpty(T t)
		{
			m_EmptyQ.Enque(t);
		}
		T PopEmpty()
		{
			return m_EmptyQ.Deque();
		}
		void PushFull(T t)
		{
			m_FullQ.Enque(t);
		}
		T PopFull()
		{
			return m_FullQ.Deque();
		}
		void CancelWaiting()
		{
			m_EmptyQ.CancelWaiting();
			m_FullQ.CancelWaiting();
		}
		unsigned int Size()
		{
			return m_FullQ.Size();
		}
		bool Full()
		{
			return m_EmptyQ.Size() == 0;
		}
		bool Empty()
		{
			return m_FullQ.Size() == 0;
		}
	private:
		WaitingQueue<T> m_EmptyQ;
		WaitingQueue<T> m_FullQ;
	};
}
}
