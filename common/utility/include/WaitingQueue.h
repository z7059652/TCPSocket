#pragma once
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "error_handling_utility.h"

namespace Titanium { namespace TIRA
{
	template<class ObjType>
	class WaitingQueue
	{
	public:
		WaitingQueue()
			: m_IsUnblock(false)
		{
		}
		ObjType Deque()
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_CV.wait(lock, [&]()
			{
				return m_IsUnblock || m_InternalQueue.size() > 0;
			});
			if (m_IsUnblock)
				throw ExceptionWithString("Deque is canceled!");

			auto ret = m_InternalQueue.front();
			m_InternalQueue.pop();
			return ret;
		}
		void Enque(const ObjType& obj)
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_InternalQueue.push(obj);
			m_CV.notify_one();
		}
		void CancelWaiting()
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_IsUnblock = true;
			m_CV.notify_all();
		}
		unsigned int Size()
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			return m_InternalQueue.size();
		}
	private:
		std::queue<ObjType> m_InternalQueue;
		std::mutex m_Mutex;
		std::condition_variable m_CV;
		bool m_IsUnblock;
	};
}
}
