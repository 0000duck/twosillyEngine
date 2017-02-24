/** 
文件名：logoutput.cpp

时间：2016年7月8日 17:56:14
内容：日志输出文件，用于对程序的调试跟踪是一个很方便的内容
作者：TwoSilly
E-MAIL：TwoSilly@foxmail.com
*/
#include <stdio.h>
#include <stdarg.h>

#include "logoutput.h"

namespace Engine {

	static int LogSwitch;   //日志开关
	static bool progressLoggingSwitch;//进程日志开关

void onLogSwitch()
{
	LogSwitch++;
}

void onProgressLoggingSwitch()
{
	progressLoggingSwitch = true;
}

void logError(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fflush(stderr);
}

void log(const char* fmt, ...)
{
	if (LogSwitch < 1)
		return;

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fflush(stderr);
}

void logProgress(const char* type, int values, int maxValue)
{
	if (!progressLoggingSwitch)
		return;

	fprintf(stderr, "Progress:%s:%i:%i\n", type, values, maxValue);
	fflush(stderr);
}

};//Engine