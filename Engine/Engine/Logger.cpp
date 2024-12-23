#include "Logger.h"

#include <windows.h>

Logger logger;

void Logger::Log(const char file[], int line, LogType type, const char* format, ...)
{
	static char tmpString1[4096];
	static char tmpString2[4096];
	static va_list ap;

	const char* filename = strrchr(file, '\\');
	if (!filename) {
		filename = strrchr(file, '/');
	}
	filename = filename ? filename + 1 : file;

	va_start(ap, format);
	vsprintf_s(tmpString1, sizeof(tmpString1), format, ap);
	va_end(ap);
	sprintf_s(tmpString2, sizeof(tmpString2), "\n%s(%d) : %s", filename, line, tmpString1);
	OutputDebugStringA(tmpString2);

	logger.AddLog(type, tmpString2);
}

void Logger::AddLog(LogType type, std::string message)
{
	message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());
	logs.push_back({ type, message });
}

void Logger::Clear()
{
	logs.clear();
}