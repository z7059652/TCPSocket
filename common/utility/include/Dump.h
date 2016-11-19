#pragma once
#include <string>

namespace Titanium { namespace TIRA
{
	std::wstring CombinePath(std::wstring const& dir, std::wstring const& file);
	std::wstring CreateDirectoryRecursive(std::wstring const& folder);
	std::wstring GetLocalAppDataFolder();
}
}
