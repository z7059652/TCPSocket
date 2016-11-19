#pragma once
#include <functional>
class RedirectPipe
{
public:
	RedirectPipe();
	~RedirectPipe();
	std::function<void(void*, unsigned int)> HIDDataReceivedEvent;
};

