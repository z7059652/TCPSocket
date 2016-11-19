#include "network_change.h"
#include "error_handling_utility.h"
using namespace Titanium::TIRA;

	static void CALLBACK s_OnNetworkAddressChange(PVOID lpParameter, BOOLEAN)
	{
		NetworkChange *networkChange = reinterpret_cast<NetworkChange*>(lpParameter);
		networkChange->OnNetworkAddressChanged();
	}

	void NetworkChange::OnNetworkAddressChanged()
	{
		m_sigNetworkAddressChanged();

		// request again
		HANDLE handle = NULL;
		NotifyAddrChange(&handle, &m_overlapped);
	}

	SignalConnection NetworkChange::RegisterNetworkAddressChangedEvent(NetworkAddressChangedAction action)
	{
		return std::move(m_sigNetworkAddressChanged.connect(action));
	}

	void NetworkChange::Stop()
	{
		if (m_hWait)
		{
			UnregisterWaitEx(m_hWait, INVALID_HANDLE_VALUE);
			m_hWait = NULL;
		}
		if (m_overlapped.hEvent)
		{
			CloseHandle(m_overlapped.hEvent);
			m_overlapped.hEvent = NULL;
		}
	}

	void NetworkChange::Start()
	{
		m_hWait = NULL;
		memset(&m_overlapped, 0, sizeof(m_overlapped));
		m_overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		ENSURE(m_overlapped.hEvent != NULL);
		ENSURE(RegisterWaitForSingleObject(&m_hWait, m_overlapped.hEvent,
			s_OnNetworkAddressChange, this, INFINITE, 0) == TRUE);
		HANDLE h;
		NotifyAddrChange(&h, &m_overlapped);
	}

	NetworkChange::~NetworkChange()
	{
		Stop();
	}

	NetworkChange::NetworkChange()
	{

	}