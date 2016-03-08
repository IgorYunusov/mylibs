#include "mylib.h"
#include "VInputDevice.h"

HKL gDefHKL = NULL;

VKeyboard::VKeyboard(void)
{
	_assoHwnd = NULL;
	memset(_keyState, 0, sizeof(_keyState));
	_enableEvent = false;
	_enableStat = true;
	_syncMode = true;
}

VKeyboard::~VKeyboard(void)
{

}

bool VKeyboard::init()
{
	return true;
}

void VKeyboard::press(BYTE key, DWORD period /* = 20 */, ULONG_PTR extra /* = O */, 
					  DWORD startTime /* = Utils::getMsTime() */ )
{
	KeyAction keyAction;
	keyAction.key = key;
	keyAction.startTime = startTime;
	keyAction.endTime = startTime + period;
	// _TRACE("BEGIN: %d, END: %d\n", keyAction.startTime, keyAction.endTime);
	keyAction.extra = extra;

	if (_enableStat) {
		MutexGuard autolock(this);
		_keyActions.push(keyAction);
	}

	DWORD now;
	now = Utils::getMsTime();
		
	if (startTime > now) {
		if (_enableEvent) {
			MsgTimer* timer = new MsgTimer(this, key, true, extra);
			timer->beginTimer(startTime - now, false);
		}

	} else {
		if (_enableEvent)
			onKeyDown(key, extra);
		
		//if (keyAction.endTime > now)
		//	Sleep(keyAction.endTime - now);
	}

	if (_enableEvent) {

		now = Utils::getMsTime();

		if (keyAction.endTime > now) {

			MsgTimer* timer = new MsgTimer(this, key, false, extra);
			timer->beginTimer(keyAction.endTime - now, false);
		} else {

			onKeyUp(key, extra);
		}
	}
}

bool VKeyboard::getState(BYTE state[256])
{
	if (!_enableStat) {

		return false;
	}

	memset(state, 0, 256);
	DWORD now = Utils::getMsTime();
	// _TRACE("NOW: %d", now);

	MutexGuard autolock(this);
	while (!_keyActions.empty()) {
		const KeyAction& keyAction = _keyActions.top();

		if (keyAction.startTime <= now) { // 已经开始
			KeyState& stat = _keyState[keyAction.key];
			if (keyAction.endTime > stat.endTime)
				stat.endTime = keyAction.endTime;

			_keyActions.pop();
		} else // 还没开始
			break;
	}

	for (int i = 0; i < 256; i ++) {

		if (_keyState[i].endTime >= now)
			state[i] |= 0x80;
		else 
			_keyState[i].endTime = 0;
	}

	return true;
}

volatile LONG gKeyDataSequence = 0x0fffffff;

size_t VKeyboard::getData(DIDEVICEOBJECTDATA* pData, size_t count)
{
	DWORD now = Utils::getMsTime();

	size_t n = 0;
	MutexGuard autolock(this);
	while (!_keyActions.empty() && n < count) {
		const KeyAction& keyAction = _keyActions.top();

		if (keyAction.startTime <= now) { // 已经开始
			KeyState& stat = _keyState[keyAction.key];
			if (keyAction.endTime > stat.endTime)
				stat.endTime = keyAction.endTime;

			try {
				pData[n].dwOfs = keyAction.key;
				pData[n].dwData = 0x80;
				pData[n].dwTimeStamp = keyAction.startTime;
				pData[n].dwSequence = InterlockedIncrement(&gKeyDataSequence);
				pData[n].uAppData = 0;

			} catch (...) {

				return 0;
			}

			_TRACE("%x Pressed", keyAction.key);

			_keyActions.pop();
			n ++;

		} else // 还没有开始
			break;
	}

	for (int i = 0; i < 256 && n < count; i ++) {

		if (_keyState[i].endTime && _keyState[i].endTime < now) {

			try {
				pData[n].dwOfs = i;
				pData[n].dwData = 0;
				pData[n].dwTimeStamp = _keyState[i].endTime;
				pData[n].dwSequence = InterlockedIncrement(&gKeyDataSequence);
				pData[n].uAppData = 0;

			} catch(...) {

				return n;
			}

			_TRACE("%x Released", i);

			n ++;

			_keyState[i].endTime = 0;
		}		
	}

	return n;
}

bool VKeyboard::hasData()
{
	DWORD now = Utils::getMsTime();

	MutexGuard autolock(this);
	while (!_keyActions.empty()) {
		const KeyAction& keyAction = _keyActions.top();

		if (keyAction.startTime <= now) { // 已经开始

			return true;

		} else // 还没有开始
			break;
	}

	for (int i = 0; i < 256; i ++) {

		if (_keyState[i].endTime && _keyState[i].endTime < now) {
			return true;
		}		
	}

	return false;
}

void VKeyboard::onKeyDown(BYTE key, ULONG_PTR extra)
{
	if (_assoHwnd != NULL) {
		::PostMessage(_assoHwnd, WM_KEYDOWN, convToVK(key), extra);
	}
}

void VKeyboard::onKeyUp(BYTE key, ULONG_PTR extra)
{
	if (_assoHwnd != NULL) {
		::PostMessage(_assoHwnd, WM_KEYUP, convToVK(key), extra);
	}
}
