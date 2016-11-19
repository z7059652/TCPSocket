#pragma once
#include <mutex>
#include <string>
#include <fstream>

namespace Titanium { namespace TIRA
{
	class Logger
	{
	public:
		static Logger& Instance();
		void WriteLine(std::string const& line);
		std::string CurrentDateTime();
	private:
		Logger();
		Logger(Logger const&);
		Logger& operator=(Logger const&);

		std::mutex m_writeLock;
		std::ofstream logFile_;
	};
}
}
