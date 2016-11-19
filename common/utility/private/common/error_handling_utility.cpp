#include "error_handling_utility.h"
using namespace Titanium::TIRA;

void MuteAllExceptions(std::function<void()> action, std::string msg)
{
	try
	{
		action();
	}
	catch (ExceptionWithString& ex)
	{
		TRACE("Exception Catched: %s %s\n", msg.c_str(), ex.what());
	}
	catch (std::exception& ex)
	{
		TRACE("Exception Catched: %s %s\n", msg.c_str(), ex.what());
	}
	catch (...)
	{
		TRACE("Exception Catched: %s\n", msg.c_str());
	}
}
