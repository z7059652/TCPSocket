#include "thread.h"
#include <thread>
#ifdef WIN32
#include "target_os.h"
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // must be 0x1000
	LPCSTR szName; // pointer to name (in user addr space)
	DWORD dwThreadID; // thread ID (-1=caller thread)
	DWORD dwFlags; // reserved for future use, must be zero
} THREADNAME_INFO;
#endif

namespace Titanium
{
	namespace TIRA
	{
		void SetThreadName(const char *szThreadName)
		{
#ifdef WIN32
			THREADNAME_INFO info;
			info.dwType = 0x1000;
			info.szName = szThreadName;
			info.dwThreadID = -1;
			info.dwFlags = 0;
			__try
			{
				RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
			}
			__except (EXCEPTION_CONTINUE_EXECUTION)
			{
			}
#endif
		}

		void sure_async(std::function<void()> callback)
		{
			std::thread th([=]()
			{
				SetThreadName("sure_aync_thread");
				try
				{
					callback();
				}
				catch (...)
				{
				}
			});
			th.detach();
		}
	}
}