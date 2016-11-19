#pragma once
#include "IRedirector.h"

class HIDRedirector :public IRedirector
{
public:
	HIDRedirector(std::shared_ptr<Titanium::IChannel> channel) ;
	~HIDRedirector();
	virtual void Start();
	virtual void Stop();
};