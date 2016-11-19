#pragma once
#include <cctype>
#include <functional>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "target_os.h"

namespace Titanium { namespace TIRA
{
	template <typename Iter>
	std::wstring ConcatAll(Iter begin, Iter end)
	{
		std::wstringstream ss;

		std::for_each(begin, end, [&](std::wstring const& s)
		{
			ss << s;
		});

		return ss.str();
	}

	template <typename CharT>
	std::vector<std::basic_string<CharT>>& Split(const std::basic_string<CharT>& s, CharT delim, std::vector<std::basic_string<CharT>>& elems)
	{
		std::basic_stringstream<CharT> ss(s);
		std::basic_string<CharT> item;
		while (std::getline(ss, item, delim))
		{
			elems.push_back(item);
		}
		return elems;
	}

	template <typename CharT>
	std::vector<std::basic_string<CharT>> Split(const std::basic_string<CharT>& s, CharT delim)
	{
		std::vector<std::basic_string<CharT>> elems;
		return Split(s, delim, elems);
	}

	template <typename T, typename CharT>
	T lexical_cast(std::basic_string<CharT> const& str)
	{
		std::basic_stringstream<CharT> ss;
		ss << str;
		T t;
		ss >> t;
		return t;
	}
	template <typename T, typename CharT>
	std::basic_string<CharT> lexical_cast(T const& t)
	{
		std::basic_stringstream<CharT> ss;
		ss << t;
		return ss.str();
	}
#ifndef __GNUC__
	template <typename CharT>
	POINT ParsePoint(std::basic_string<CharT> const& str)
	{
		typedef std::basic_string<CharT> str_t;
		std::vector<str_t> xy = Split(str, L',');
		assert(xy.size() == 2);

		POINT pt = { lexical_cast<int>(xy[0]), lexical_cast<int>(xy[1]) };
		return pt;
	}
#endif // __GNUC__

	template <typename CharT>
	std::vector<int> ParseInts(std::basic_string<CharT> const& str)
	{
		typedef std::basic_string<CharT> str_t;
		std::vector<str_t> intStrs = Split(str, L',');

		std::vector<int> ints;

		std::transform(intStrs.begin(), intStrs.end(), std::back_inserter(ints), [](str_t str) -> int { return lexical_cast<int>(str); });

		return ints;
	}

	template <typename T>
	std::wstring ToString(T const& t)
	{
		std::wstringstream ss;
		ss << t;
		return ss.str();
	}

	template <typename CharT, typename T>
	std::basic_string<CharT> ToString(T const& t, int width, CharT padding)
	{
		std::basic_stringstream<CharT> ss;
		ss << std::setw(width) << std::setfill(padding) << t;
		std::basic_string<CharT> s;
		ss >> s;
		ENSURE(s.length() == width)(s.c_str());
		return s;
	}

	inline bool IsEqualIgnoreCase(std::wstring str1, std::wstring str2)
	{
		std::transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
		std::transform(str2.begin(), str2.end(), str2.begin(), ::tolower);

		return str1 == str2;
	}

	inline bool StartsWith(std::wstring const& str, std::wstring const& prefix)
	{
		if (prefix.length() > str.length())
		{
			return false;
		}

		return str.compare(0, prefix.length(), prefix) == 0;
	}

	inline bool EndsWith(std::wstring const& str, std::wstring const& suffix)
	{
		if (suffix.length() > str.length())
		{
			return false;
		}

		return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
	}

	class StringHelper
	{
	public:
		static inline std::string & TrimL(std::string &s)
		{
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			return s;
		}


		static inline std::string &TrimR(std::string &s)
		{
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
			return s;
		}


		static inline std::string &Trim(std::string &s)
		{
			return TrimL(TrimR(s));
		}

		static void Split(const std::string& src, const std::string& separator, std::vector<std::string>& result, bool removeEmpty = true)
		{
			size_t start = 0;
			size_t end = src.find(separator);
			while (end != std::string::npos)
			{
				if (!removeEmpty && end == start)
				{
					result.push_back("");
				}
				else if (end > start)
				{
					std::string item = src.substr(start, end - start);
					result.push_back(item);
				}

				start = end + separator.size();
				if (start >= src.size())
				{
					if (!removeEmpty)
						result.push_back("");
					return;
				}
				end = src.find(separator, start);
			}
			if (start < src.size())
				result.push_back(src.substr(start));
		}

		static inline int FindInStrArray(const std::string strs[], const std::string& target, size_t size)
		{
			for (unsigned int i = 0; i < size; i++)
			{
				if (strs[i].compare(target) == 0)
					return i;
			}
			return -1;
		}

		static void SplitWStr(const std::wstring& src, const std::wstring& separator, std::vector<std::wstring>& result, bool removeEmpty = true)
		{
			size_t start = 0;
			size_t end = src.find(separator);
			while (end != std::wstring::npos)
			{
				if (!removeEmpty && end == start)
				{
					result.push_back(L"");
				}
				else if (end > start)
				{
					std::wstring item = src.substr(start, end - start);
					result.push_back(item);
				}

				start = end + separator.size();
				if (start >= src.size())
				{
					if (!removeEmpty)
						result.push_back(L"");
					return;
				}
				end = src.find(separator, start);
			}
			if (start < src.size())
				result.push_back(src.substr(start));
		}
	};
}
}

