#include "AudioSource.h"
extern "C" _declspec(dllimport) void(*gSendAudioFormatInfo)(void *data, unsigned int size);
Titanium::IChannel * g_Channel;
void OnAudioFormatInfodataReceive(void* buf, unsigned int len)
{
	g_Channel->Send((unsigned char*)&len, sizeof(len));
	g_Channel->Send((unsigned char*)buf, len);
}
AudioSource::AudioSource(std::shared_ptr<Titanium::IChannel> channel) :IRdpSource(channel)
{
	gSendAudioFormatInfo = OnAudioFormatInfodataReceive;
	g_Channel = m_Channel.get();
}
AudioSource::~AudioSource()
{
	Stop();
	g_Channel = nullptr;
}
