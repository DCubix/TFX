#include "Logger.h"

#include <fstream>
#include <assert.h>

#include "termcolor.hpp"

namespace tfx {
#ifdef TFX_DEBUG
	Logger Logger::logger = Logger();
	std::ofstream& Logger::logFile = std::ofstream();
#else
	std::ofstream& Logger::logFile = std::ofstream("Log_" + Util::currentDateTimeNoFormat() + ".txt");
	Logger Logger::logger = Logger(Logger::logFile);
#endif

	Logger::Logger() : m_output(std::cout) {
	}

	Logger::Logger(std::ostream& output) : m_output(output) {
		if (m_output.bad()) {
			assert(false);
		}
	}

	Logger::~Logger() {
		m_output.flush();
		static std::stringstream closed_flag;
		m_output.rdbuf(closed_flag.rdbuf());
		if (logFile) {
			logFile.close();
		}
	}
	
	void Logger::print(LogLevel level, const char* file, const char* function, int line, const String& msg) {
		// [12/12/2017 23:45] => [ERROR] [func@33] Test error!
		String prefx = "[" + Util::currentDateTime() + "] => ";
		String filen = file;
		filen = filen.replace(R"(\)", "/");
		filen = filen.subStr(filen.lastIndexOf(String("/"))+1);

#ifdef TFX_DEBUG
		m_output << termcolor::green << termcolor::dark;
#endif

		m_output << prefx.str();

#ifdef TFX_DEBUG
		switch (level) {
			case LogLevel::Debug: m_output << termcolor::cyan; break;
			case LogLevel::Info: m_output << termcolor::blue; break;
			case LogLevel::Warning: m_output << termcolor::yellow; break;
			case LogLevel::Error: m_output << termcolor::red; break;
			case LogLevel::Fatal: m_output << termcolor::magenta; break;
		}
#endif
		switch (level) {
			case LogLevel::Debug: m_output << "[DEBG]"; break;
			case LogLevel::Info: m_output << "[INFO]"; break;
			case LogLevel::Warning: m_output << "[WARN]"; break;
			case LogLevel::Error: m_output << "[ERROR]"; break;
			case LogLevel::Fatal: m_output << "[FATAL]"; break;
		}

#ifdef TFX_DEBUG
		m_output << termcolor::reset;
#endif

		m_output << " [" << filen << "(" << function << " @ " << line << ")] " << msg << std::endl;
	}

}