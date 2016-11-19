#include "Dump.h"
#include "ScopeGuard.h"
#include "target_os.h"
#include <iostream>
#include <assert.h>
namespace Titanium
{
	namespace TIRA
	{
		std::wstring CombinePath(std::wstring const& dir, std::wstring const& file)
		{
			WCHAR finalPath[MAX_PATH];
			auto result = PathCombine(finalPath, dir.c_str(), file.c_str());
			if (result == NULL)
			{
				throw std::runtime_error("cannot combine the path!");
			}
			return finalPath;
		}

		std::wstring CreateDirectoryRecursive(std::wstring const& folder)
		{
			SHCreateDirectory(NULL, folder.c_str());
			return folder;
		}

		std::wstring GetSpecialFolder(REFKNOWNFOLDERID rfid)
		{
			typedef HRESULT(STDAPICALLTYPE * fnptr_SHGetKnownFolderPath)(__in REFKNOWNFOLDERID rfid, __in DWORD dwFlags, __in_opt HANDLE hToken, __deref_out PWSTR *ppszPath);
			fnptr_SHGetKnownFolderPath fnSHGetKnownFolderPath = (fnptr_SHGetKnownFolderPath)GetProcAddress(GetModuleHandle(TEXT("shell32")), "SHGetKnownFolderPath");

			if (fnSHGetKnownFolderPath != NULL)
			{
				PWSTR p;
				auto hr = fnSHGetKnownFolderPath(rfid, 0, NULL, &p);
				if (hr != S_OK)
				{
					throw std::runtime_error("cannot get folder path!");
				}
				std::wstring p1(p);
				CoTaskMemFree(p);
				return p1;
			}
			else
			{
				// SHGetKnownFolderPath is not found, in XP
				WCHAR p[MAX_PATH];
				if (!(rfid == FOLDERID_LocalAppData || rfid == FOLDERID_LocalAppDataLow || rfid == FOLDERID_RoamingAppData))
				{
					throw std::runtime_error("only support 'LocalAppData', 'AppDataLow' and 'RoamingAppData'.");
				}

				auto hr = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, p);
				if (hr != S_OK)
				{
					throw std::runtime_error("cannot get folder path!");
				}
				return p;
			}
		}

		std::wstring GetLocalAppDataFolder()
		{
			return GetSpecialFolder(FOLDERID_LocalAppData);
		}
	}
}