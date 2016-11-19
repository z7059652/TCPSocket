#include "AsyncBase.h"
#include "thread.h"
using namespace Titanium::TIRA;

bool AsyncBase::CanLoopContinue()
{
	std::unique_lock<std::mutex> lock(m_operationMutex);
	m_loopNotify.wait(lock, [&]() { return m_status != Status_Paused; });
	return (m_status == Status_Started);
}

void AsyncBase::UnBlockWorkingThread()
{

}

Ti_Worker_Status AsyncBase::AsyncBaseStatus()
{
	return m_status;
}

bool AsyncBase::IsAsyncBaseInError()
{
	return m_status == Status_Error;
}

bool AsyncBase::IsAsyncBaseRunning()
{
	return m_status != Status_Error && m_status != Status_Stopped;
}

void AsyncBase::ResumeAsyncBase()
{
	std::lock_guard<std::mutex> lg(m_operationMutex);
	if (m_status != Status_Paused)
	{
		return;
	}
	m_status = Status_Started;
	m_loopNotify.notify_all();
}

void AsyncBase::PauseAsyncBase()
{
	std::lock_guard<std::mutex> lg(m_operationMutex);
	if (m_status != Status_Started)
	{
		return;
	}
	m_status = Status_Paused;
}

void AsyncBase::StopAsyncBase()
{
	{
		std::lock_guard<std::mutex> lg(m_operationMutex);
		if (m_status != Status_Stopped)
		{
			m_status = Status_Stopped;

			UnBlockWorkingThread();

			m_loopNotify.notify_all();
		}
	}

	std::lock_guard<std::mutex> lock(m_threadOpMutex);
	if (m_workingThread)
	{
		if (m_workingThread->joinable())
		{
			m_workingThread->join();
		}
		m_workingThread.reset();
	}
}

void AsyncBase::StartAsyncBase()
{
	std::lock_guard<std::mutex> lg(m_operationMutex);
	if (m_status == Status_Stopped)
	{
		m_status = Status_Started;
		std::lock_guard<std::mutex> lock(m_threadOpMutex);
		m_workingThread.reset(new std::thread([&]()
		{
			try
			{
				SetThreadName(m_ThreadName.c_str());
				Workloop();

				//set the status to stopped;					
			}
			catch (ExceptionWithString &e)
			{
				TRACE("%s: %s\n", m_ThreadName.c_str(), e.what());
				m_status = Status_Error;
			}
		}));
	}
	else if (m_status == Status_Paused)
	{
		m_status = Status_Started;
		m_loopNotify.notify_all();
	}
}

AsyncBase::~AsyncBase(void)
{
	MuteAllExceptions([&]() { StopAsyncBase(); });
}

AsyncBase::AsyncBase(const std::string& name)
	: m_status(Status_Stopped)
	, m_ThreadName(name)
{
}