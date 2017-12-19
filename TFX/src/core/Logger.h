#ifndef TFX_CORE_LOG_H
#define TFX_CORE_LOG_H

#include "Util.h"
#include <fstream>
#include <iostream>

namespace tfx {
	enum LogLevel {
		Debug = 0,
		Info,
		Warning,
		Error,
		Fatal
	};

	class Logger {
	public:
		Logger();
		Logger(std::ostream& output);
		~Logger();

		void print(LogLevel level, const char* file, const char* function, int line, const String& msg);

		static Logger& getSingleton() { return logger; }
	private:
		std::ostream& m_output;

		static Logger logger;
		static std::ofstream& logFile;
	};
}

#define LOGGER Logger::getSingleton()
#define Log(msg) LOGGER.print(LogLevel::Debug, __FILE__, __FUNCTION__, __LINE__, msg)
#define LogInfo(msg) LOGGER.print(LogLevel::Info, __FILE__, __FUNCTION__, __LINE__, msg)
#define LogWarning(msg) LOGGER.print(LogLevel::Warning, __FILE__, __FUNCTION__, __LINE__, msg)
#define LogError(msg) LOGGER.print(LogLevel::Error, __FILE__, __FUNCTION__, __LINE__, msg)
#define LogFatal(msg) LOGGER.print(LogLevel::Fatal, __FILE__, __FUNCTION__, __LINE__, msg)

#endif // TFX_CORE_LOG_H