#pragma once
#include <type_traits>
#include <vector>
#include "error_handling_utility.h"
#include "rectdefs.h"

namespace Titanium { namespace TIRA
{
	class ReadableSource
	{
	public:
		virtual int ReadBytes(unsigned char* buffer, int bufLen) = 0;
		virtual void CancelRead() = 0;
		virtual bool IsSourceInError() = 0;
	public:
		template<typename _Ty>
		typename std::enable_if<std::is_pod<_Ty>::value, void>::type
			Read(_Ty *val)
		{

			auto readTo = (unsigned char*)(val);
			unsigned int count = sizeof(_Ty);

			while (count > 0)
			{
				auto readLength = ReadBytes(readTo, count);
				ENSURE(readLength != 0);
				count -= readLength;
				readTo += readLength;
			}
		}
		template<typename _Ty>
		inline typename std::enable_if<std::is_pod<_Ty>::value, _Ty>::type
			Read()
		{
			_Ty ret;
			Read(&ret);
			return ret;
		}
#ifndef __GNUC__
		template<>
		void Read(TI_Resolution * val)
		{
			val->m_Width = Read<unsigned int>();
			val->m_Height = Read<unsigned int>();
		}
#endif
		template<typename _CharT>
		void Read(std::basic_string<_CharT> *str)
		{
			auto length = ReadUInt32();

			str->resize(length);
			if (length > 0)
			{
				auto readTo = (unsigned char*)str->c_str();
				ReadBytes(readTo, sizeof(_CharT) * length);
			}
		}

		template<typename _Ty>
		typename std::enable_if<std::is_pod<_Ty>::value>::type
			Read(std::vector<_Ty> *vtr)
		{
			auto length = ReadUInt32();
			vtr->resize(length);
			if (length > 0)
			{
				auto st = vtr->begin();
				while (st != vtr->end())
				{
					Read(&st[0]);
					++st;
				}
			}
		}

		bool ReadBoolean();
		unsigned char ReadByte();
		float ReadSingle();
		double ReadDouble();
		short ReadInt16();
		int ReadInt32();
		long long ReadInt64();
		char ReadChar();
		wchar_t ReadWchar();
		unsigned short ReadUInt16();
		unsigned int ReadUInt32();
		unsigned long long ReadUInt64();
		TI_Resolution ReadResolution();
	};
}
}
