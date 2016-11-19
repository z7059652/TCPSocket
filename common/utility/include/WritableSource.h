#pragma once
#include <type_traits>
#include <vector>
#include <algorithm>
#include "rectdefs.h"

namespace Titanium { namespace TIRA
{
	class WritableSource
	{
	public:
		virtual void WaitTillSourceIdeal() = 0;
		virtual void WriteBytes(const unsigned char* buffer, int bufLen) = 0;
		virtual void Cancel() = 0;
		virtual bool IsSourceInError() = 0;
	public:
		template<typename _Ty>
		inline void Write(const _Ty &val)
		{
			unsigned int count = sizeof(val);
			WriteBytes((unsigned char*)&val, count);
		}

#ifndef __GNUC__
		template<>
		void Write(const TI_Resolution & rsl)
		{
			Write(rsl.m_Width);
			Write(rsl.m_Height);
		}
#endif
#ifdef __GNUC__
		void Write(const std::string& str)
		{
			unsigned int length = str.length();

			Write(length);
			if(length > 0)
			{
				auto cStr = (unsigned char*)str.c_str();	
				WriteBytes(cStr, length);
			}
		}

		void Write(const std::wstring& wstr)
		{
			std::string str(2 * wstr.length(), '\0');
			for (int i=0; i<wstr.length(); i++){
				str[i*2] = wstr[i];
				str[i*2+1] = '\0';
			}

			unsigned int length = wstr.length();
			Write(length);
			if(length > 0)
			{
				auto cStr = (unsigned char*)str.c_str();
				WriteBytes(cStr, length * 2);
			}
		}
#else

		template<typename _CharT>
		void Write(const std::basic_string<_CharT>& str)
		{
			auto length = (unsigned int)str.length();

			Write(length);
			if (length > 0)
			{
				auto cStr = (unsigned char*)str.c_str();
				WriteBytes(cStr, sizeof(_CharT) * length);
			}
		}

#endif
		template<typename _Ty>
		typename std::enable_if<std::is_pod<_Ty>::value>::type
			Write(const std::vector<_Ty>& vtr)
		{
			auto length = (unsigned int)vtr.size();
			Write(length);
			std::for_each(vtr.begin(), vtr.end(), [&](const _Ty & ty)
			{
				Write(ty);
			});
		}
	};
}
}
