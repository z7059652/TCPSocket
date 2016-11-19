#pragma once
#include "Component.h"
#include "ISink.h"
extern "C"
{
#include "rdpdr_main.h"
}
class DeviceRedirecr : public Component, public ISink
{
public:
	DeviceRedirecr(rdpdrPlugin* rdpdrplugin);
	virtual ~DeviceRedirecr();
	virtual void Start();
	virtual void Stop();
protected:
	virtual void Process();
	rdpdrPlugin* m_rdpdrPlugin;
private:
	bool m_IsAlive;
};

