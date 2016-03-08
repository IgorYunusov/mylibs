#pragma once

#include "Thread/Lock.h"
#include "Utils/Time.h"
#include "Utils/Timer.h"

extern HKL gDefHKL;

inline BYTE DIK2VK(BYTE key)
{
	if (gDefHKL == NULL) {

		gDefHKL = GetKeyboardLayout(GetCurrentThreadId());
	}

	return (BYTE )MapVirtualKeyEx(key, 1, gDefHKL);
}

inline BYTE VK2DIK(BYTE key)
{
	if (gDefHKL == NULL) {

		gDefHKL = GetKeyboardLayout(GetCurrentThreadId());
	}

	return (BYTE )MapVirtualKeyEx(key, 0, gDefHKL);
}

class VKeyboard: public Mutex
{
public:
	VKeyboard(void);
	~VKeyboard(void);

	bool init();

	void setAssoWindow(HWND hwnd)
	{
		_assoHwnd = hwnd;
	}
	
	HWND getAssoWindow() const 
	{
		return _assoHwnd;
	}

	virtual BYTE convToVK(BYTE code)
	{
		return code;
	}

	virtual BYTE convToDIK(BYTE code)
	{
		return code;
	}

	virtual bool transToVKState(LPBYTE stat, size_t len)
	{
		return true;
	}

	virtual bool transToDIKState(LPBYTE stat, size_t len)
	{
		return true;
	}

	void press(BYTE key, DWORD period = 20, ULONG_PTR extra = 0, DWORD startTime = Utils::getMsTime());

	bool getState(BYTE state[256]);
	size_t getData(DIDEVICEOBJECTDATA* pData, size_t count);
	bool hasData();

protected:
	virtual void onKeyDown(BYTE key, ULONG_PTR extra);
	virtual void onKeyUp(BYTE key, ULONG_PTR extra);

public:
	bool		_enableEvent;
	bool		_enableStat;
	bool		_syncMode;

protected:
	struct KeyAction {

		bool operator < (const KeyAction& keyAction) const
		{
			return startTime < keyAction.startTime;
		}

		BYTE		key;
		DWORD		startTime;
		DWORD		endTime;
		ULONG_PTR	extra;
	};

	struct KeyState {
		
		DWORD		endTime;
	};

	struct MsgTimer: public ExactTimer {

		MsgTimer(VKeyboard* vk, BYTE keycode, bool down, ULONG_PTR extra)
		{
			vkbd = vk;
			key = keycode;
			isDown = down;
		}

		virtual void onTimer()
		{
			if (isDown)
				vkbd->onKeyDown(key, extra);
			else
				vkbd->onKeyUp(key, extra);
		}

		VKeyboard*		vkbd;
		BYTE			key;
		bool			isDown;
		ULONG_PTR		extra;
	};

	typedef std::priority_queue<KeyAction> KeyActions;
	KeyActions	_keyActions;
	HWND		_assoHwnd;

	KeyState	_keyState[256];
};
