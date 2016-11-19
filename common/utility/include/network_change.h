#pragma once
#include "target_os.h"
#include "SignalConnection.h"

namespace Titanium { namespace TIRA
{
	typedef std::function<void()> NetworkAddressChangedAction;
	class NetworkChange
	{
	public:
		NetworkChange();;
		~NetworkChange();;
		void Start();
		void Stop();
		SignalConnection RegisterNetworkAddressChangedEvent(NetworkAddressChangedAction action);
		void OnNetworkAddressChanged();
	private:
		OVERLAPPED m_overlapped;
		HANDLE m_hWait;
		Signal<void()> m_sigNetworkAddressChanged;
	};
}
}
