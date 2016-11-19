#pragma once
#include <functional>

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)
#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)

namespace Titanium { namespace TIRA
{
	class ScopeGuard
	{
	public:
		explicit ScopeGuard(std::function<void()> onExitScope)
			: onExitScope_(onExitScope), dismissed_(false)
		{
		}

		~ScopeGuard()
		{
			try
			{
				if (!dismissed_)
				{
					onExitScope_();
				}
			}
			catch (...) {}
		}

		void Dismiss()
		{
			dismissed_ = true;
		}
	private:
		std::function<void()> onExitScope_;
		bool dismissed_;

		// noncopyable
	private:
		ScopeGuard(ScopeGuard const&);
		ScopeGuard& operator=(ScopeGuard const&);
	};
}
}
