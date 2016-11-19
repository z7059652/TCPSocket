#pragma once
#include <string>

namespace Titanium { namespace TIRA
{
	struct TI_GUID
	{
		unsigned long  Data1;
		unsigned short Data2;
		unsigned short Data3;
		unsigned char  Data4[8];
		TI_GUID();
		bool operator==(const TI_GUID& id) const;
		bool operator!=(const TI_GUID& id) const;
		void Clear();
		static TI_GUID Empty();
		std::wstring ToWString() const;
		std::string ToString() const;
	};
	TI_GUID CreateGUID();
}
}
