#include "RdpSetting.h"
#include <sstream>
RdpSetting::RdpSetting(const std::string & server, const std::string & domain, const std::string & user, const std::string & password)
	: _server(server)
	, _domain(domain)
	, _user(user)
	, _password(password)
{
}

std::string RdpSetting::ToString() const
{
	std::ostringstream os;
	os << "/audio-mode:0" 
		<< " /u:" << _user
		<< " /p:" << _password
		<< " /v:" << _server
		<< " /d:" << _domain
		<< " /w:" << _resolution.w
		<< " /h:" << _resolution.h
		<< " /gfx"
		<< " /multitouch"
		<< " /fast-path:0" 
		<< " /vc:rdpsnd";
	return os.str();
}
