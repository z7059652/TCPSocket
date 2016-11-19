#include "UacDesktopGuard.h"
#include "windows.h"
using namespace Titanium::TIRA;

UacDesktopGuard::UacDesktopGuard()
	: m_CurrentDesktop(NULL)
	, m_InputDesktop(NULL)
{
	m_InputDesktop = OpenInputDesktop(0, FALSE,
		DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
		DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
		DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
		DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
	if (m_InputDesktop != NULL)
	{
		m_CurrentDesktop = GetThreadDesktop(GetCurrentThreadId());
		SetThreadDesktop((HDESK)m_InputDesktop);
	}
}

UacDesktopGuard::~UacDesktopGuard()
{
	if (m_InputDesktop != NULL)
	{
		SetThreadDesktop((HDESK)m_CurrentDesktop);
		CloseDesktop((HDESK)m_InputDesktop);
	}
}