#pragma once

namespace Titanium { namespace TIRA
{
	class UacDesktopGuard
	{
	public:
		UacDesktopGuard();
		~UacDesktopGuard();
	private:
		void *m_CurrentDesktop;
		void *m_InputDesktop;
	};
}
}
