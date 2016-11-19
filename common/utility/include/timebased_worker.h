#pragma once
#include <functional>
#include <exception>
#include <atomic>
#include "AsyncBase.h"
#include "wait_timer.h"
#include "SignalConnection.h"

namespace Titanium { namespace TIRA
{
	class timebased_worker_cancel : std::exception {};
	class TimeBasedWorker : public AsyncBase
	{
		typedef std::function<void(void*)> ThreadingTimer_Callback;
	public:
		TimeBasedWorker(unsigned int wait_duration, std::string thread_name = "threading_timer");
		~TimeBasedWorker();
		SignalConnection RegisterCallback(ThreadingTimer_Callback callback, void* pContext = nullptr);
		void Start();
		void Stop();
		void SetDuration(unsigned int wait_duration);
	protected:
		virtual void Workloop();
		virtual void UnBlockWorkingThread();
	private:
		wait_timer _wait_timer;
		std::atomic<unsigned int> _wait_duration;
		Signal<void(void*)> _callback;
		void* _pContext;
	};
}
}
