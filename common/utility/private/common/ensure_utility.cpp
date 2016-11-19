#include "ensure_utility.h"
#include "target_os.h"
#include "Logger.h"
#include "wstr2str.h"
#include "dump.h"
#include <algorithm>
#include <cassert>
#include <stdexcept>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "dbghelp.lib") 
using namespace Titanium::TIRA;

std::wstring FormatMessage(std::wstring const& expr, TContextVarMap const& contextVars, std::wstring const& file, long line)
{
	std::wstringstream ss;
	ss << expr << L" [" << file << L", " << line << L"]\n";

	if (!contextVars.empty())
	{
		std::for_each(contextVars.begin(), contextVars.end(), [&](TContextVarMap::const_reference p)
		{
			ss << L"    " << p.first << L" = " << p.second << L"\n";
		});
	}

	return ss.str();
}

void ThrowBehavior::operator()(std::wstring const& expr, TContextVarMap const& contextVars, std::wstring const& file, long line)
{
	throw ExceptionWithString(FormatMessage(expr, contextVars, file, line), L"");
}

void AssertBehavior::operator()(std::wstring const& expr, TContextVarMap const& contextVars, std::wstring const& file, long line)
{
#ifndef NDEBUG
#ifdef RAISE_ASSERT
	_wassert(FormatMessage(expr, contextVars, file, line).c_str(), file.c_str(), line);
#endif
#endif
	ThrowBehavior()(expr, contextVars, file, line);
}

void ThrowWithoutDumpBehavior::operator()(std::wstring const& expr, TContextVarMap const& contextVars, std::wstring const& file, long line)
{
	throw std::runtime_error(wstr2str(FormatMessage(expr, contextVars, file, line)));
}

void TraceBehavior::operator()(std::wstring const& expr, TContextVarMap const& contextVars, std::wstring const& file, long line)
{
	time_t now = time(NULL);
	tm *t = localtime(&now);
	std::wstringstream wss;
	wss << '[' << t->tm_hour << ':' << t->tm_min << ':' << t->tm_sec << "] ";
	wss << FormatMessage(expr, contextVars, file, line);

	wprintf(wss.str().c_str());
}

void WarningBehavior::operator()(std::wstring const& expr, TContextVarMap const& contextVars, std::wstring const& file, long line)
{
	Logger::Instance().WriteLine(wstr2str(FormatMessage(expr, contextVars, file, line)));
}