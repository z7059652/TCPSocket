#include "wait_timer.h"
using namespace Titanium::TIRA;

void wait_timer::cancel()
{
	std::unique_lock<std::mutex> lock(_mutex_);
	_cv_.notify_all();
}

bool wait_timer::wait(unsigned int ms)
{
	std::unique_lock<std::mutex> lock(_mutex_);
	return (_cv_.wait_for(lock, std::chrono::milliseconds(ms)) == std::cv_status::timeout);
}
