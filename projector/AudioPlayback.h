#pragma once
#include "Component.h"
#include "ISink.h"
extern "C"
{
#include "rdpsnd_main.h"
}
class AudioPlayback : public Component, public ISink
{
public:
	AudioPlayback(rdpsndPlugin* rdpdrplugin);
	virtual ~AudioPlayback();
	virtual void Start();
	virtual void Stop();
protected:
	virtual void Process();
	rdpsndPlugin* m_rdpsndPlugin;
private:
	bool m_IsAlive;
};

