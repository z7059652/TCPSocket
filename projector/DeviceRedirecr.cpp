#include "DeviceRedirecr.h"
#include <iostream>
extern "C"
{
#include "rdpdr_main.h"
	UINT rdpdr_process_receive(rdpdrPlugin* rdpdr, wStream* s);
}
DeviceRedirecr::DeviceRedirecr(rdpdrPlugin* rdpdrplugin) :m_IsAlive(false)
{
	if (rdpdrplugin == NULL)
		throw std::exception("INVALID INPUT");
	m_rdpdrPlugin = rdpdrplugin;
}

DeviceRedirecr::~DeviceRedirecr()
{
}
void DeviceRedirecr::Start()
{
	if (m_rdpdrPlugin)
		m_IsAlive = true;
	else
		throw std::exception("Channel Initial Failed");
	Component::Start();
}
void DeviceRedirecr::Stop()
{
	m_IsAlive = false;
	Stream s;
	_in->Push(s);
	Component::Stop();
}

void DeviceRedirecr::Process()
{
	if (m_IsAlive)
	{
		auto inStream = _in->Pop();
		wStream *s = Stream_New(inStream.buffer.get(), inStream.m_bufferSize);
		if (s->length>1)
		rdpdr_process_receive(m_rdpdrPlugin, s);
		Stream_Free(s, FALSE);
	}
}