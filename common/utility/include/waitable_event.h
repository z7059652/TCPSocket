#pragma once
#include <mutex>
#include <condition_variable>

namespace Titanium { namespace TIRA
{
	class event_base
	{
	public:
		/// true if is signaled
		event_base(bool initialState)
			: _isSignaled(initialState)
		{
		}
		virtual bool wait(unsigned int ms) = 0;
		virtual void wait() = 0;
		void reset()
		{
			std::unique_lock<std::mutex> lock(_mutex_);
			_isSignaled = false;
		}
		void set()
		{
			std::unique_lock<std::mutex> lock(_mutex_);
			_isSignaled = true;
			_cv_.notify_all();
		}
	protected:
		std::mutex _mutex_;
		std::condition_variable _cv_;
		bool _isSignaled;
	};

	class auto_reset_event : public event_base
	{
	public:
		/// true if is signaled
		auto_reset_event(bool initialState)
			: event_base(initialState)
		{
		}
		virtual bool wait(unsigned int ms)
		{
			std::unique_lock<std::mutex> lock(_mutex_);
			if (_cv_.wait_for(lock, std::chrono::milliseconds(ms), [&]() { return _isSignaled; }))
			{
				_isSignaled = false;
				return true;
			}
			else
			{
				return false;
			}
		}
		void wait()
		{
			std::unique_lock<std::mutex> lock(_mutex_);
			_cv_.wait(lock, [&]() { return _isSignaled; });
			_isSignaled = false;
		}

	};

	class manual_reset_event : public event_base
	{
	public:
		/// true if is signaled
		manual_reset_event(bool initialState)
			: event_base(initialState)
		{
		}
		virtual bool wait(unsigned int ms)
		{
			std::unique_lock<std::mutex> lock(_mutex_);
			if (_cv_.wait_for(lock, std::chrono::milliseconds(ms), [&]() { return _isSignaled; }))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		void wait()
		{
			std::unique_lock<std::mutex> lock(_mutex_);
			_cv_.wait(lock, [&]() { return _isSignaled; });
		}

	};
}
}
