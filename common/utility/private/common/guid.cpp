#include "guid.h"
#include <stdio.h>
using namespace Titanium::TIRA;

TI_GUID::TI_GUID()
{
	memset(this, 0, sizeof(TI_GUID));
}

std::string TI_GUID::ToString() const
{
	const size_t guidSize = 37;
	char szGuid[guidSize];
	if (sprintf(szGuid, "%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		Data1,
		Data2,
		Data3,
		Data4[0], Data4[1],
		Data4[2], Data4[3], Data4[4], Data4[5],
		Data4[6], Data4[7]) > 0)
	{
		return szGuid;
	}
	return "";
}

std::wstring TI_GUID::ToWString() const
{
	const size_t guidSize = 37;
	wchar_t szGuid[guidSize];
	if (swprintf(szGuid, guidSize, L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		Data1,
		Data2,
		Data3,
		Data4[0], Data4[1],
		Data4[2], Data4[3], Data4[4], Data4[5],
		Data4[6], Data4[7]) > 0)
	{
		return szGuid;
	}
	return L"";
}

TI_GUID TI_GUID::Empty()
{
	TI_GUID guid;
	return guid;
}

void TI_GUID::Clear()
{
	memset(this, 0, sizeof(TI_GUID));
}

bool TI_GUID::operator!=(const TI_GUID& id) const
{
	return 0 != memcmp(this, &id, sizeof(TI_GUID));
}

bool TI_GUID::operator==(const TI_GUID& id) const
{
	return 0 == memcmp(this, &id, sizeof(TI_GUID));
}
namespace Titanium
{
	namespace TIRA
	{

#ifdef _WIN32
#include "objbase.h"
		TI_GUID CreateGUID()
		{
			GUID guid;
			CoCreateGuid(&guid);
			return *(TI_GUID*)&guid;
		}
#elif defined(__GNUC__)
		TI_GUID CreateGUID()
		{
			static unsigned long globalIncreasement = 1;
			TI_GUID guid;
			guid.Data1 = globalIncreasement++;
			return guid;
		}
#endif
	}
}