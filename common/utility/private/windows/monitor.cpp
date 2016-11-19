#include "monitor.h"
using namespace Titanium::TIRA;

BOOL CALLBACK MonitorHelper::MonitorEnumProcPrimary(__in HMONITOR hMonitor, __in HDC hdcMonitor, __in LPRECT lprcMonitor, __in LPARAM dwData)
{
	std::vector<HMONITOR>& monitors = *(std::vector<HMONITOR>*)dwData;
	monitors.push_back(hMonitor);
	return true;
}

MonitorHandleWithRect MonitorHelper::GetMonitor(bool isPrimay)
{
	std::vector<HMONITOR> monitors;
	EnumDisplayMonitors(NULL, NULL, &MonitorHelper::MonitorEnumProcPrimary, (LPARAM)&monitors);
	MonitorHandleWithRect ret;

	auto result = std::find_if(monitors.begin(), monitors.end(), [&](HMONITOR hMonitor)->bool
	{
		MONITORINFOEX mifex;
		mifex.cbSize = sizeof(MONITORINFOEX);
		if (GetMonitorInfo(hMonitor, &mifex))
		{
			bool primayMonitor = ((mifex.dwFlags&MONITORINFOF_PRIMARY) != 0);
			if (isPrimay == primayMonitor)
			{

				// ruigao, used for mapping touch device to the display
				// get the monitor ID
				DISPLAY_DEVICE device = { 0 };
				device.cb = sizeof(device);

				EnumDisplayDevices(mifex.szDevice, 0, &device, DISPLAY_DEVICE_ATTACHED_TO_DESKTOP);

				StringCchCopy(ret.m_DeviceID, ARRAYSIZE(ret.m_DeviceID), device.DeviceID);
				// end

				ret.m_hMonitor = hMonitor;
				ret.m_Region = mifex.rcMonitor;
				ret.m_WorkRegion = mifex.rcWork;
				return true;
			}
		}
		return false;
	});

	if (result == monitors.end())
		throw std::exception("Cannot get resolution.");
	return ret;
}

MonitorHandleWithRect MonitorHelper::GetMonitor(HMONITOR hMonitor)
{
	MonitorHandleWithRect ret = { 0 };

	MONITORINFOEX mifex;
	mifex.cbSize = sizeof(MONITORINFOEX);
	if (GetMonitorInfo(hMonitor, &mifex))
	{
		// ruigao, used for mapping touch device to the display
		// get the monitor ID
		DISPLAY_DEVICE device = { 0 };
		device.cb = sizeof(device);

		EnumDisplayDevices(mifex.szDevice, 0, &device, DISPLAY_DEVICE_ATTACHED_TO_DESKTOP);

		StringCchCopy(ret.m_DeviceID, ARRAYSIZE(ret.m_DeviceID), device.DeviceID);
		// end
		ret.m_hMonitor = hMonitor;
		ret.m_Region = mifex.rcMonitor;
		ret.m_WorkRegion = mifex.rcWork;
	}
	return ret;
}

bool MonitorHelper::IsExtendMode()
{
	try
	{
		GetMonitor(false);
		return true;
	}
	catch (std::exception& e)
	{
		static_cast<void>(e);
		return false;
	}
}

MonitorHandleWithRect MonitorHelper::GetExtendedMonitor()
{
	return GetMonitor(false);
}

MonitorHandleWithRect MonitorHelper::GetPrimaryMonitor()
{
	return GetMonitor(true);
}