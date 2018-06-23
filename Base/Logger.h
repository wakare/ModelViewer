#pragma once
#include <iostream>
#include <memory>

enum class LogType
{
	INFO  = 0,
	DEBUG = 1,
	WARN  = 2,
	ERR   = 3,
	FATAL = 4
};

enum OutputDevice
{
	CONSOLE = 0,
	WINDOW	= 1
};

template<typename ...Args>
inline std::string Format(const std::string & format, Args ...args)
{
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

class Logger
{
public:
	static void Log(LogType eType, std::string string, OutputDevice outputDevice = OutputDevice::CONSOLE);
	static void Log(LogType eType, const char* szString, OutputDevice outputDevice = OutputDevice::CONSOLE);
};


