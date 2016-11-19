#pragma once

#include "Logger.h"
#include "ensure_utility.h"

#define WIDEN2(x) L##x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#define ENSURE_A(x) ENSURE_OP(x, B)
#define ENSURE_B(x) ENSURE_OP(x, A)
#define ENSURE_OP(x, next) \
	ENSURE_A.PrintContextVar(WIDEN(#x), (x)).ENSURE_##next

// ENSURE
#ifdef _DEBUG
#define ENSURE(expr) \
	if((expr)) static_assert(std::is_same<decltype(expr), bool>::value, "ENSURE(expr) can only be used on bool expression"); \
	else Titanium::TIRA::MakeEnsureAssert(WIDEN(#expr)).PrintLocation(__WFILE__, __LINE__).ENSURE_A
#else
#define ENSURE(expr) \
	if((expr)) static_assert(std::is_same<decltype(expr), bool>::value, "ENSURE(expr) can only be used on bool expression"); \
	else Titanium::TIRA::MakeEnsureThrowWithoutDump(WIDEN(#expr)).PrintLocation(__WFILE__, __LINE__).ENSURE_A
#endif

// ENSURE_WIN32
#ifdef _WIN32
#include "target_os.h"
inline std::wstring Win32ErrorMessage(DWORD dwError)
{
	LPVOID pText = 0;
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, dwError, MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US), (LPWSTR)&pText, 0, NULL);
	if(pText != NULL)
	{
		std::wstring msg((LPWSTR)pText);
		ENSURE(LocalFree(pText) == NULL);
		return msg;
	}
	return L"Unknown Error";
}
#endif
#ifdef WIN32
#define ENSURE_WIN32(exp) ENSURE(exp)(Win32ErrorMessage(GetLastError()))
#define ENSURE_SUCCEEDED(hr) \
	if(SUCCEEDED(hr)) static_assert(std::is_same<decltype(hr), HRESULT>::value, "ENSURE_SUCCEEDED(hr) can only be used on HRESULT"); \
	else ENSURE(SUCCEEDED(hr))(Win32ErrorMessage(hr))
#elif defined(__GNUC__)
#define ENSURE_WIN32(exp)
#define ENSURE_SUCCEEDED(hr) ENSURE((hr)>=0)
#endif

#ifdef _DEBUG
#ifdef _ANDROID
#include <android/log.h>
#define TRACE(...) __android_log_print(ANDROID_LOG_VERBOSE, "Titanium", __VA_ARGS__)
#else
#define TRACE(...) printf(##__VA_ARGS__)
#endif
#else
#define TRACE(...)
#endif

// LOGGING
#define LOGGING(msg)  Logger::Instance().WriteLine(msg)

// exception
void MuteAllExceptions(std::function<void()> action, std::string msg = "MuteAllExceptions!");