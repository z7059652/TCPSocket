#pragma once
#include <string>
#include <stdexcept>

namespace Titanium { namespace TIRA
{
	class ExceptionWithString : public virtual std::runtime_error
	{
	public:
		ExceptionWithString(std::string const& msg);
		ExceptionWithString(std::wstring const& msg, std::wstring miniDumpFileFullPath = L"");
		std::wstring GetMiniDumpFilePath();
	private:
		std::wstring wstr;
	};
}
}
