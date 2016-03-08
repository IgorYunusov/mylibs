#pragma once

////////////////////////////////////////////////////////////////////////////
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

class Timer {
public:

	Timer()
	{
		_timerId = 0;
		_delay = 0;
		_periodic = false;
	}

	virtual ~Timer()
	{

	}

	virtual void onTimer() = 0;
	virtual bool beginTimer(unsigned long delay, bool periodic = true) = 0;
	virtual void endTimer() = 0;

	UINT getDelay() const
	{
		return _delay;
	}

	bool isPeriodic() const 
	{
		return _periodic;
	}

	UINT_PTR getTimerId() const
	{
		return _timerId;
	}

	virtual void release()
	{
		delete this;
	}

protected:

	UINT_PTR		_timerId;
	unsigned long	_delay;
	bool			_periodic;

};

class ExactTimer: public Timer {
public:

	virtual bool beginTimer(unsigned long delay, bool periodic = true);
	virtual void endTimer();

protected:

	static void CALLBACK timerCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, 
		DWORD_PTR dw1, DWORD_PTR dw2);

};

#include "Thread/Lock.h"

// 必须运行在有消息循环的程序
class WinTimer: public Timer {
public:

	virtual bool beginTimer(unsigned long delay, bool periodic = true);
	virtual void endTimer();

protected:

	static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

private:
	static Mutex	_timerMapLock;
	static std::map<UINT_PTR, WinTimer* > _timerMap;
};
