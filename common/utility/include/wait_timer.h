#pragma once
#include <mutex>
#include <condition_variable>

namespace Titanium { namespace TIRA
{
	class wait_timer
	{
	public:
		// return value: true if timeout
		bool wait(unsigned int ms);
		void cancel();
	private:
		std::mutex _mutex_;
		std::condition_variable _cv_;
	};
}
}
