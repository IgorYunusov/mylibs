#pragma once

enum LogLevel {
	LogDebug, 
	LogInfo, 
	LogWarning, 
	LogError,
	LogDisable, 
};

class Log {
public:
	Log()
	{
		_logLevel = LogDebug;
	}

	void setLevel(LogLevel level)
	{
		_logLevel = level;
	}

	LogLevel getLevel() const 
	{
		return _logLevel;
	}

	void log(LogLevel level, const char* fmt, ...);

protected:
	virtual void output(const char* str);

protected:
	LogLevel		_logLevel;
};

extern Log* glog;
