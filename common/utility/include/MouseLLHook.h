#pragma once
#include "target_os.h"
#include "AsyncBase.h"

namespace Titanium
{
	class MouseLLHook
	{
	public:
		static MouseLLHook& Instance();
	private:
		MouseLLHook();
		~MouseLLHook();
	public:
		void Start();
		void Stop();
		HHOOK GetHookHandle();
		POINT GetPosition();
		void UpdatePosition(const POINT& pos);
	private:
		void InstallHook();
		void UninstallHook();
		void MessageLoop();

	private:
		static void ThreadProc(LPVOID lpParm);
		HHOOK m_hMouseHook;
        std::atomic_int m_posX;
        std::atomic_int m_posY;
		
        HANDLE m_hThread;
		DWORD m_dwThreadId;
		std::unique_ptr<std::thread> m_pThread;
	};
}

