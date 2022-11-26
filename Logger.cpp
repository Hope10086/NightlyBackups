#include "Logger.h"
#include <cstdarg>
#include "driverlog.h"
#include "bindings.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstdarg>
#include <iostream>
#include <string>
#include <windows.h>
using namespace std;
void _logSV(const char* format, va_list args, string Type)
{   
    SYSTEMTIME sys{0};
    GetLocalTime(&sys);//本地时间
    string sys_time = "\n#"
        + to_string(sys.wYear)+"-" 
        + to_string(sys.wMonth) +"-"
        + to_string(sys.wDay) +"-" 
        + to_string(sys.wHour) +":"
        + to_string(sys.wMinute) +":" 
        + to_string(sys.wSecond) +":"
        + to_string(sys.wMilliseconds) + "#"
        +Type;
    string BeginTime = 
          to_string(sys.wMonth)+"-"
        + to_string(sys.wDay)+"-"
        + to_string(sys.wHour);
    FILE* fpLog = NULL;
    char buf[1024*4]; 
    string LogFile= "D:\\AX\\Logs\\Debug\\Debug"+ BeginTime + ".txt";
    errno_t err = fopen_s(&fpLog, LogFile.c_str(), "at+");
    if (err != 0)
    {
        printf("Failed to open");
        //exit(0);
    }
    else if (fpLog)
    {
        vsnprintf(buf, 2048, format, args);
        fprintf(fpLog, sys_time.c_str());
        fprintf(fpLog, buf);
    }
    fclose(fpLog);
}

void _log(const char *format, va_list args, void (*logFn)(const char *), bool driverLog = false)
{
	char buf[1024];
	int count = vsnprintf(buf, sizeof(buf), format, args);
	if (count > (int)sizeof(buf))
		count = (int)sizeof(buf);
	if (count > 0 && buf[count - 1] == '\n')
		buf[count - 1] = '\0';

	logFn(buf);

	//TODO: driver logger should concider current log level
#ifndef ALVR_DEBUG_LOG
	if (driverLog)
#endif
		DriverLogVarArgs(format, args);
}

Exception MakeException(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	Exception e = FormatExceptionV(format, args);
	va_end(args);

	return e;
}

void Error(const char *format, ...)
{  
	string Error_Type ="Error:";
	va_list args;
	va_start(args, format);
	_log(format, args, LogError, true);
	_logSV(format, args,Error_Type);
	va_end(args);
}

void Warn(const char *format, ...)
{   string Warn_Type ="Warn:";
	va_list args;
	va_start(args, format);
	_log(format, args, LogWarn, true);
	_logSV(format, args,Warn_Type);
	va_end(args);
}

void Info(const char *format, ...)
{  
	string Info_Type = "Info:";
	va_list args;
	va_start(args, format);
	// Don't log to SteamVR/writing to file for info level, this is mostly statistics info
	_log(format, args, LogInfo);
	_logSV(format, args, Info_Type);
	va_end(args);
}

void Debug(const char *format, ...)
{
// Use our define instead of _DEBUG - see build.rs for details.
	string Debug_Type = "Debug:";
	va_list args;
	va_start(args, format);
	_logSV(format, args, Debug_Type);
#ifdef ALVR_DEBUG_LOG
	_log(format, args, LogDebug);
#else
	(void)format;
#endif
	va_end(args);
}
