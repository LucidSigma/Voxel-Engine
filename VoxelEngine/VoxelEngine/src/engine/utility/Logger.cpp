#include "Logger.h"

void Logger::Log(const std::string& message)
{
	s_logFile << message << "\n";
}