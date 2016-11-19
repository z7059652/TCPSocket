#include "timebased_worker.h"
#include "error_handling_utility.h"
using namespace Titanium::TIRA;

void TimeBasedWorker::UnBlockWorkingThread()
{
	_wait_timer.cancel();
}

void TimeBasedWorker::Workloop()
{
	while (CanLoopContinue())
	{
		if (_wait_timer.wait(_wait_duration))
		{
			try
			{
				_callback(_pContext);
			}
			catch (timebased_worker_cancel& e)
			{
				static_cast<void>(e);
				return;
			}
		}
		else
		{
			return;
		}
	}
}

void TimeBasedWorker::SetDuration(unsigned int wait_duration)
{
	_wait_duration = wait_duration;
}

void TimeBasedWorker::Stop()
{
	StopAsyncBase();
}

void TimeBasedWorker::Start()
{
	ENSURE(_callback.size() > 0)("ThreadingTimer: Callback must be set!");
	StartAsyncBase();
}

SignalConnection TimeBasedWorker::RegisterCallback(ThreadingTimer_Callback callback, void* pContext /*= nullptr*/)
{
	return std::move(_callback.connect(callback));
}

TimeBasedWorker::~TimeBasedWorker()
{
	Stop();
}

TimeBasedWorker::TimeBasedWorker(unsigned int wait_duration, std::string thread_name /*= "threading_timer"*/)
	: AsyncBase(thread_name)
	, _wait_duration(wait_duration)
	, _pContext(nullptr)
{
}