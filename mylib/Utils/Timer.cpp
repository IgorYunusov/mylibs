#include "mylib.h"
#include "Timer.h"

#define _REDEFINE_ONEXACTTIMER

////////////////////////////////////////////////////////////////////////////
// class ExactTimer

/*
static void _onExactTimer(ExactTimer* )
{

}

void (* onExactTimerPtr)(ExactTimer* ) = &_onExactTimer;
*/

void CALLBACK ExactTimer::timerCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, 
								   DWORD_PTR dw1, DWORD_PTR dw2)
{
	ExactTimer *thisPtr = (ExactTimer *)dwUser;
	assert(thisPtr != NULL);
	// (*onExactTimerPtr)(thisPtr);
	thisPtr->onTimer();
}

bool ExactTimer::beginTimer(unsigned long delay, bool periodic /* = true */)
{
	const UINT resolution = 10;

	_timerId = ::timeSetEvent(delay, resolution, timerCallback, (DWORD_PTR )this, 
		periodic ? TIME_PERIODIC : TIME_ONESHOT);

	if (_timerId == 0)
		return false;

	_delay = delay;
	_periodic = periodic;

	return true;
}

void ExactTimer::endTimer()
{
	::timeKillEvent((UINT )_timerId);
	_timerId = 0;
}

//////////////////////////////////////////////////////////////////////////

std::map<UINT_PTR, WinTimer* > WinTimer::_timerMap;
Mutex WinTimer::_timerMapLock;

bool WinTimer::beginTimer(unsigned long delay, bool periodic /* = true */ )
{
	_timerId = ::SetTimer(NULL, 0, delay, TimerProc);
	if (_timerId == 0)
		return false;
	_delay = delay;
	_periodic = periodic;

	_timerMapLock.lock();
	_timerMap[_timerId] = this;
	_timerMapLock.unlock();
	return true;
}

void WinTimer::endTimer()
{
	::KillTimer(NULL, _timerId);

	_timerMapLock.lock();
	_timerMap.erase(_timerId);
	_timerMapLock.unlock();
}

VOID CALLBACK WinTimer::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	_timerMapLock.lock();
	std::map<UINT_PTR, WinTimer* >::iterator it = _timerMap.find(idEvent);
	if (it == _timerMap.end()) {

		_timerMapLock.unlock();
		assert(false);
		return;
	}

	WinTimer* timer = it->second;
	timer->onTimer();
	if (!timer->_periodic) {
		timer->endTimer();
		timer->release();
	}

	_timerMapLock.unlock();	
}
