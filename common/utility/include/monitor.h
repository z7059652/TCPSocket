#pragma once
#include <Windows.h>
#include <Strsafe.h> 
#include <vector>
#include <algorithm>

namespace Titanium { namespace TIRA
{
	typedef struct
	{
		HMONITOR m_hMonitor;
		RECT m_Region;
		RECT m_WorkRegion;
		// ruigao
		TCHAR  m_DeviceID[128];
	}MonitorHandleWithRect;

	//BUGUBG:: only support two monitor so far.
	class MonitorHelper
	{
	public:
		static MonitorHandleWithRect GetMonitor(HMONITOR hMonitor);
		static MonitorHandleWithRect GetPrimaryMonitor();
		static MonitorHandleWithRect GetExtendedMonitor();
		static bool IsExtendMode();
	protected:
		static MonitorHandleWithRect GetMonitor(bool isPrimay);
		static BOOL CALLBACK MonitorEnumProcPrimary(
			__in  HMONITOR hMonitor,
			__in  HDC hdcMonitor,
			__in  LPRECT lprcMonitor,
			__in  LPARAM dwData
			);
	};
}
}
