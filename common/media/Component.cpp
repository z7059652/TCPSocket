#include "Component.h"

Component::Component()
	: _cancelled(false)
	, _paused(false)
{
}

Component::~Component()
{
}

bool Component::CanLoop()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cv.wait(lock, [this]() {return _cancelled || !_paused; });
	return !_cancelled;
}

void Component::Start()
{
	_thread = std::make_shared<std::thread>([this]()
	{
		try
		{
			while (CanLoop())
				Process();
		}
		catch (const std::exception&)
		{
		}
	});
}

void Component::Stop()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_cancelled = true;
	_cv.notify_all();
	lock.unlock();
	if (_thread != nullptr){
		_thread->join();
		_thread = nullptr;
	}
}

void Component::Pause()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_paused = true;
}

void Component::Resume()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_paused = false;
	_cv.notify_all();
}
