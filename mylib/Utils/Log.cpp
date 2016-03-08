#include "mylib.h"
#include "Log.h"
#include <stdarg.h>

static Log defLog;
Log* glog = &defLog;

//////////////////////////////////////////////////////////////////////////

void Log::output(const char* str)
{
	OutputDebugStringA(str);
}

void Log::log(LogLevel level, const char* fmt, ...)
{
	if (level < _logLevel)
		return;

	static char* levelName[] = {
		"DBG", 
		"INF", 
		"WAR", 
		"ERR", 
	};

	SYSTEMTIME st;
	GetLocalTime(&st);

	va_list vlist;
	va_start(vlist, fmt);
	char buf[2048];

	int len = sprintf_s(buf, sizeof(buf), "~%04d[%02d:%02d:%02d.%03d][%s] ", GetCurrentThreadId(), 
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, levelName[level]);

	len = vsprintf_s(&buf[len], sizeof(buf) - len, fmt, vlist);
	strcat_s(buf, sizeof(buf), "\n");

	output(buf);
}
