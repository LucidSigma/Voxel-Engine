#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

class Logger
{
private:
	inline static std::ofstream s_logFile{ "logs/log.txt", std::ios_base::out | std::ios_base::app };

public:
	Logger() = delete;
	~Logger() noexcept = delete;

	static void Log(const std::string& message);
};

#endif