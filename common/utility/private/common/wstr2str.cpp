#include "wstr2str.h"
#include <stdlib.h>
#include <vector>

namespace Titanium
{
	namespace TIRA
	{
		std::string wstr2str(const std::wstring& wstr)
		{
			size_t len = wstr.length();
			if (len == 0)
				return "";
			std::string str(len, '\0');

			for (int i = 0; i < len; i++)
			{
				str[i] = (char)wstr[i];
			}
			return str;
		}

		std::wstring str2wstr(const std::string& str)
		{
			size_t len = str.length();
			std::wstring wstr(len, L'\0');
			if (len == 0)
				return L"";

			for (int i = 0; i < len; i++)
			{
				wstr[i] = (wchar_t)str[i];
			}
			return wstr;
		}
	}
}