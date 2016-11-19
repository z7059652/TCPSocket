#pragma once
#include "Component.h"
#include "ISink.h"
extern "C"
{
#include "drdynvc_main.h"
}
class D2DRender : public Component, public ISink
{
public:
	D2DRender(drdynvcPlugin* g_drdynvc);
	virtual ~D2DRender();
	virtual void Start();
	virtual void Stop();
protected:
	virtual void Process();
	IWTSVirtualChannelCallback* m_channelCallback;
private:
	bool m_IsAlive;
};

