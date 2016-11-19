#include <iostream>
#include "AudioPlayback.h"
extern "C"
{
	UINT rdpsnd_recv_pdu(rdpsndPlugin* rdpsnd, wStream* s);
}
AudioPlayback::AudioPlayback(rdpsndPlugin* RdpsndPlugin) :m_IsAlive(false)
{
	if (RdpsndPlugin == NULL)
		throw std::exception("INVALID INPUT");
	m_rdpsndPlugin = RdpsndPlugin;
}

AudioPlayback::~AudioPlayback()
{
}
void AudioPlayback::Start()
{
	if (m_rdpsndPlugin)
		m_IsAlive = true;
	else
		throw std::exception("Channel Initial Failed");
	Component::Start();
}
void AudioPlayback::Stop()
{
	m_IsAlive = false;
	Stream s;
	_in->Push(s);
	Component::Stop();
}

void AudioPlayback::Process()
{
	if (m_IsAlive)
	{
		auto inStream = _in->Pop();
		wStream *s = Stream_New(inStream.buffer.get(), inStream.m_bufferSize);
		if (s->length>1)
		rdpsnd_recv_pdu(m_rdpsndPlugin, s);
		Stream_Free(s, FALSE);
	}
}