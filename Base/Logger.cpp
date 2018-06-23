#include "Logger.h"

void Logger::Log(LogType eType, std::string string, OutputDevice outputDevice)
{
	Log(eType, string.c_str(), outputDevice);
}

void Logger::Log(LogType eType, const char* szString, OutputDevice outputDevice)
{
	if (outputDevice == OutputDevice::CONSOLE)
		std::cout << szString << std::endl;

	else if (outputDevice == OutputDevice::WINDOW)
		;
}
