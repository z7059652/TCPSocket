#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <Windows.h>
#include <windef.h>
#include <DbgHelp.h>
#include <Shlwapi.h>
#include <ShlObj.h>

// HID
#include <Objbase.h>
#include <setupapi.h>

// Network
#include <iphlpapi.h>
#include <Ws2tcpip.h>

//compiler key word
#include <intrin.h>

#define ALIGN(n) __declspec(align(n))

#define  TiCreateEvent(handle)  (handle = CreateEvent(NULL, false, false, NULL))
#define  TiWaitForSingleObject(a)  WaitForSingleObject(a, INFINITE)
#define  TiSetEvent(handle)  SetEvent(handle) 
#define  TiCloseEvent(handle)  CloseHandle(handle)
#define  TiInterlockedCompareExchange(a,b,c)  InterlockedCompareExchange(a,b,c)
#define  TiInterlockedIncrement(a)  InterlockedIncrement(a)
#define  TiInterlockedDecrement(a) InterlockedDecrement(a)
#elif defined(__GNUC__)
typedef unsigned long       DWORD;
typedef unsigned short      WORD;
#define INFINITE (-1)
#endif

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif