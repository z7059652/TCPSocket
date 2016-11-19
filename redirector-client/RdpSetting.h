#pragma once
#include <string>
#include "Resolution.h"

class RdpSetting
{
public:
	RdpSetting(const std::string &server, const std::string &domain, const std::string &user, const std::string &password);
	std::string ToString() const;
	Resolution _resolution;
private:
	std::string _server;
	std::string _domain;
	std::string _user;
	std::string _password;
};

