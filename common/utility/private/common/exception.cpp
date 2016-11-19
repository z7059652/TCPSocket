#include "exception.h"
#include "wstr2str.h"
using namespace Titanium::TIRA;

std::wstring ExceptionWithString::GetMiniDumpFilePath()
{
	return wstr;
}
ExceptionWithString::ExceptionWithString(const std::string& msg)
	: std::runtime_error(msg)
{
}
ExceptionWithString::ExceptionWithString(std::wstring const& msg, std::wstring miniDumpFileFullPath) : std::runtime_error(wstr2str(msg))
, wstr(std::move(miniDumpFileFullPath))
{
}
