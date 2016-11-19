#pragma once
#include <condition_variable>
#include <memory>
#include <mutex>
#include <chrono>
#include <atomic>
#include <thread>
#include "error_handling_utility.h"

namespace Titanium { namespace TIRA
{
	enum Ti_Worker_Status
	{
		Status_Stopped,
		Status_Started,
		Status_Paused,
		Status_Error
	};

	class AsyncBase
	{
	public:
		AsyncBase(const std::string& name);
		virtual ~AsyncBase(void);
	protected:
		void StartAsyncBase();
		void StopAsyncBase();
		void PauseAsyncBase();
		void ResumeAsyncBase();
		bool IsAsyncBaseRunning();
		bool IsAsyncBaseInError();
		Ti_Worker_Status AsyncBaseStatus();
		//
		// de-blocking the working thread here if there is any blocking operation in you workloop.
		//
		virtual void UnBlockWorkingThread();
		virtual bool CanLoopContinue();
		/*
		virtual void Workloop()
		{
		// variables definition here

		// your main loop, should looks like this. the CanLoopContinue will handle Stop, Pause and Resume for you.
		while(CanLoopContinue())
		{
		// you looping code
		}
		}
		*/
		virtual void Workloop() = 0;
	protected:
		Ti_Worker_Status m_status;
		std::unique_ptr<std::thread> m_workingThread;
		std::mutex m_operationMutex;
		std::mutex m_threadOpMutex;
		std::condition_variable m_loopNotify;
	private:
		std::string m_ThreadName;
	};
}
}
