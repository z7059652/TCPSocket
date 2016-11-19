#include "D2DRender.h"
#include <iostream>
extern "C"
{
#include"rdpgfx_main.h"
#include "wf_client.h"
IWTSVirtualChannel* dvcman_find_channel_by_id(IWTSVirtualChannelManager* pChannelMgr, UINT32 ChannelId);
}
D2DRender::D2DRender(drdynvcPlugin* g_drdynvc) :m_IsAlive(false)
{
	if (g_drdynvc == NULL)
		throw std::exception("INVALID INPUT");
	DVCMAN_CHANNEL* channel = (DVCMAN_CHANNEL*)dvcman_find_channel_by_id(g_drdynvc->channel_mgr, 6);
	m_channelCallback = channel->channel_callback;
}

D2DRender::~D2DRender()
{
}
void D2DRender::Start()
{
	if (m_channelCallback)
		m_IsAlive = true;
	else
		throw std::exception("Channel Initial Failed");
	Component::Start();
}
void D2DRender::Stop()
{
	m_IsAlive = false;
	Stream s;
	_in->Push(s);
	Component::Stop();
}

void D2DRender::Process()
{
	if (m_IsAlive)
	{
		auto inStream = _in->Pop();
		wStream *s = Stream_New(inStream.buffer.get(), inStream.m_bufferSize);
		if (s->length > 1)
		rdpgfx_on_data_received_Core(m_channelCallback, s);
		Stream_Free(s, FALSE);
	}
}