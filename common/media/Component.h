#pragma once
#include <thread>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Component
{
public:
	Component();
	virtual ~Component();

	virtual void Start();
	virtual void Stop();
	virtual void Pause();
	virtual void Resume();
protected:
	virtual void Process() = 0;
private:
	bool CanLoop();
	std::shared_ptr<std::thread> _thread;
	bool _cancelled;
	bool _paused;
	std::mutex _mutex;
	std::condition_variable _cv;
};