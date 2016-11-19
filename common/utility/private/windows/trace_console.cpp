#include "trace_console.h"
#include "target_os.h"
#include <fstream>

namespace Titanium
{
	namespace TIRA
	{
		void OpenTraceConsole()
		{
			BOOL con = AllocConsole();
			if (!con)
			{
				return;
			}
			freopen("CONOUT$", "w+t", stdout);
		}

		void CloseTraceConsole()
		{
			FreeConsole();
		}

		void ToggleTraceConsole()
		{
			if (GetConsoleWindow() == NULL)
			{
				OpenTraceConsole();
			}
			else
			{
				CloseTraceConsole();
			}
		}
	}
}