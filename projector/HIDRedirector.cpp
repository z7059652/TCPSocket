#include "HIDRedirector.h"
#include "windows.h"
#include "iostream"
#include "RedirectPipe.h"
#include "rdpStreamingDef.h"
extern "C" _declspec(dllimport) void(*gmSendHIDData)(void *data, unsigned int size);
RedirectPipe* g_HIDPipe = nullptr;
void OnHIDdataReceive(void* buf, unsigned int len)
{
	if (g_HIDPipe)
	{
		g_HIDPipe->HIDDataReceivedEvent(buf, len);
	}
}
HIDRedirector::HIDRedirector(std::shared_ptr<Titanium::IChannel> channel) :IRedirector(channel)
{
}

HIDRedirector::~HIDRedirector()
{
}

void HIDRedirector::Start()
{
	 _Pipe = std::make_shared<RedirectPipe>();
	 g_HIDPipe = _Pipe.get();
	 gmSendHIDData = OnHIDdataReceive;
	 _Pipe->HIDDataReceivedEvent = [this](void *data, unsigned int size)
	 {
		 std::lock_guard<std::mutex> lg(m_mux);
		 if (_RedirectChannel)
		 {
			 try
			 {
				 _RedirectChannel->Send((const unsigned char*)data, size);
			 }
			 catch (std::exception &e)
			 {
				 g_HIDPipe = nullptr;
				 _RedirectChannel.reset();
			 }
		 }
	 };
}

void HIDRedirector::Stop()
{
	std::lock_guard<std::mutex> lg(m_mux);
	if (_RedirectChannel)
		_RedirectChannel->Stop();
	g_HIDPipe = nullptr;
}