#include "IRedirector.h"

IRedirector::IRedirector(std::shared_ptr<Titanium::IChannel> channel) :_RedirectChannel(channel)
{
}

IRedirector::~IRedirector()
{
}
