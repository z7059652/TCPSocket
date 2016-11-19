#pragma once
#include <thread>
#include <condition_variable>

namespace Titanium { namespace TIRA
{
	void SetThreadName(const char *szThreadName);
	void sure_async(std::function<void()> callback);
}
}
