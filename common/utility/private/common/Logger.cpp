#include "Logger.h"
#include <iomanip>
#include <sstream>
#include <ctime>
#include <time.h>
#include "wstr2str.h"
#include "Dump.h"
using namespace Titanium::TIRA;

Logger::Logger() : logFile_(wstr2str(CombinePath(CreateDirectoryRecursive(CombinePath(GetLocalAppDataFolder(), L"Titanium")), L"ErrorLog.txt")), std::ios_base::app)
{
	logFile_ << "Start A new Applicaton" << std::endl;
}

std::string Logger::CurrentDateTime()
{
	time_t rawtime;
	std::time(&rawtime);
	std::stringstream ss;
	tm *timeinfo = std::localtime(&rawtime);
	ss << '[' << timeinfo->tm_yday << '-' << timeinfo->tm_mon << '-' << timeinfo->tm_mday << ']';
	return ss.str();
}

void Logger::WriteLine(std::string const& line)
{
	std::lock_guard<std::mutex> lock(m_writeLock);
	logFile_ << CurrentDateTime() << line << std::endl;
}

Logger& Logger::Instance()
{
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	static Logger instance_;
	return instance_;
}
