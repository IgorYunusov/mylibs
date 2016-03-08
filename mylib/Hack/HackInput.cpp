// SimInput.cpp : 定义 DLL 应用程序的入口点。
//

#include "mylib.h"
#include "HackInput.h"
#include "MyDirectInput8.h"
#include "Utils/VInputDevice.h"
#include <Utils/Utils.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

VKeyboard* gVKeyboard = NULL;

//////////////////////////////////////////////////////////////////////////

int myGetKeyState(BYTE* stat);

//////////////////////////////////////////////////////////////////////////

typedef HRESULT (WINAPI *DirectInput8Create_t)(HINSTANCE hinst, 
											   DWORD dwVersion, 
											   REFIID riidltf, 
											   LPVOID * ppvOut, 
											   LPUNKNOWN punkOuter);

static DirectInput8Create_t Real_DirectInput8Create = NULL;

HRESULT WINAPI MyDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut,
									LPUNKNOWN punkOuter)
{
	// Let the world know we're working.
	MessageBeep(MB_ICONINFORMATION);

	_TRACE(("DirectInput-Hook: MyDirectInput8Create called.\n"));

	HRESULT hr = Real_DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);	

	if(SUCCEEDED(hr))
		*ppvOut = new MyDirectInput8(reinterpret_cast<IDirectInput8*>(*ppvOut));

	return hr;
}

bool HookDInput()
{
	LPSTR DllName = "DINPUT8.DLL";
	LoadLibraryA(DllName);

	PBYTE tmp = (PBYTE )DetourFindFunction(DllName, "DirectInput8Create");

	_TRACE(("HookDInput(), tmp = %p\n"), tmp);

	if (tmp == NULL)
		return false;

	Real_DirectInput8Create = (DirectInput8Create_t ) DetourFunction(
		(PBYTE )tmp, (PBYTE )MyDirectInput8Create);

	_TRACE( ("HookDInput(), Real_DirectInput8Create = %p, MyDirectInput8Create = %p\n"), 
		tmp, &MyDirectInput8Create);

	return true;
}

//////////////////////////////////////////////////////////////////////////

#if defined(DETOURS_VERSION) && DETOURS_VERSION == 20100 

SHORT (__stdcall * Real_GetAsyncKeyState)(int a0)
	= GetAsyncKeyState;

SHORT (__stdcall * Real_GetKeyState)(int a0)
	= GetKeyState;

BOOL (__stdcall * Real_GetKeyboardState)(PBYTE a0)
	= GetKeyboardState;

#else // #if defined(DETOURS_VERSION) && DETOURS_VERSION == 20100 

DETOUR_TRAMPOLINE(SHORT __stdcall Real_GetAsyncKeyState(int a0),
				  GetAsyncKeyState);

DETOUR_TRAMPOLINE(SHORT __stdcall Real_GetKeyState(int a0),
				   GetKeyState);

DETOUR_TRAMPOLINE(BOOL __stdcall Real_GetKeyboardState(PBYTE a0),
				   GetKeyboardState);

#endif // #if defined(DETOURS_VERSION) && DETOURS_VERSION == 20100 

SHORT __stdcall Mine_GetAsyncKeyState(int a0)
{
	_TRACE("GetAsyncKeyState(%x)\n", a0);

	SHORT rv = 0;
	__try {
		rv = Real_GetAsyncKeyState(a0);
	} __finally {
		_TRACE("GetAsyncKeyState() -> %lx\n", rv);
	};

	BYTE state[256];
	int n = myGetKeyState(state);
	if (n > 0) {
		
		gVKeyboard->transToVKState(state, n);

		if (state[a0 & 0xff] & 0x80)
			rv |= 0x80;
	}

	return rv;
}

SHORT __stdcall Mine_GetKeyState(int a0)
{
	_TRACE("GetKeyState(%lx)\n", a0);

	SHORT rv = 0;
	__try {
		rv = Real_GetKeyState(a0);
	} __finally {
		_TRACE("GetKeyState() -> %lx\n", rv);
	};

	BYTE state[256];
	int n = myGetKeyState(state);
	if (n > 0) {

		gVKeyboard->transToVKState(state, n);

		if (state[a0 & 0xff] & 0x80)
			rv |= 0x80;
	}

	return rv;
}

BOOL __stdcall Mine_GetKeyboardState(PBYTE a0)
{
	_TRACE("GetKeyboardState(%lx)\n", a0);

	BOOL rv = 0;
	__try {
		rv = Real_GetKeyboardState(a0);
	} __finally {
		_TRACE("GetKeyboardState() -> %lx\n", rv);
	};

	if (!rv)
		return FALSE;

	BYTE state[256];
	int n = myGetKeyState(state);
	if (n > 0) {

		gVKeyboard->transToVKState(state, n);
		for (int i = 0; i < n; i ++) {
			if (state[i] & 0x80)
				a0[i] |= 0x80;
		}
	}

	return rv;
}

bool HookUser32()
{
#if defined(DETOURS_VERSION) && DETOURS_VERSION == 20100 

	DetourTransactionBegin();
	DetourAttach(&(PVOID& )Real_GetAsyncKeyState, Mine_GetAsyncKeyState);
	DetourAttach(&(PVOID& )Real_GetKeyState, Mine_GetKeyState);
	DetourAttach(&(PVOID& )Real_GetKeyboardState, Mine_GetKeyboardState);
	DetourTransactionCommit();

#else // #if defined(DETOURS_VERSION) && DETOURS_VERSION == 20100 
	if (!DetourFunctionWithTrampoline((PBYTE)Real_GetAsyncKeyState,	(PBYTE)Mine_GetAsyncKeyState))
		return false;

	_TRACE("HookUser32(), Real_GetAsyncKeyState = %p, Mine_GetAsyncKeyState = %p\n", 
		Real_GetAsyncKeyState, &Mine_GetAsyncKeyState);

	if (!DetourFunctionWithTrampoline((PBYTE)Real_GetKeyState, (PBYTE)Mine_GetKeyState))
		return false;

	_TRACE("HookUser32(), Real_GetKeyState = %p, Mine_GetKeyState = %p\n", 
		Real_GetKeyState, &Mine_GetKeyState);

	DetourFunctionWithTrampoline((PBYTE)Real_GetKeyboardState,
		(PBYTE)Mine_GetKeyboardState);
#endif // #if defined(DETOURS_VERSION) && DETOURS_VERSION == 20100 
	return true;
}

int myGetKeyState(BYTE* stat)
{
	if (gVKeyboard != NULL) {
		if (gVKeyboard->getState(stat))
			return 256;
	}

	return 0;
}

size_t myGetKeyData(LPDIDEVICEOBJECTDATA data, size_t count)
{
	if (gVKeyboard != NULL) {

		//__try {
			return gVKeyboard->getData(data, count);

		//} __except(EXCEPTION_EXECUTE_HANDLER) {
			return 0;
		//}
	}

	return 0;
}

bool myHasData()
{
	if (gVKeyboard == NULL)
		return false;

	return gVKeyboard->hasData();
}

HRESULT (WINAPI IDirectInputDevice8A::* Real_IDirectInputDevice8_GetDeviceStateA)(DWORD,LPVOID);
// HRESULT (WINAPI IDirectInputDevice8W::* Real_IDirectInputDevice8_GetDeviceStateW)(DWORD,LPVOID);

HRESULT (WINAPI IDirectInputDevice8A::* Real_IDirectInputDevice8_GetDeviceDataA)(DWORD size, 
																				 LPDIDEVICEOBJECTDATA data, 
																				 LPDWORD numElements, DWORD flags);

HRESULT (WINAPI IDirectInputDevice8A::* Real_IDirectInputDevice8_Acquire)();

class IDirectInputDevice8DetourA: public IDirectInputDevice8A {
public:

	bool IsKeyboard()
	{

		DIDEVCAPS info;
		info.dwSize = sizeof(info);
		HRESULT hr = GetCapabilities(&info);
		return (info.dwDevType & 0xff) == DI8DEVTYPE_KEYBOARD;
	}

	HRESULT WINAPI Mine_Keyboard_GetDeviceState(DWORD size, LPVOID data)
	{
		_TRACE(__FUNCTION__);

		HRESULT hr = (this->*Real_IDirectInputDevice8_GetDeviceStateA)(size, data);

		if(SUCCEEDED(hr) && size == sizeof(BYTE) * 256)
		{
			BYTE* keys = static_cast<BYTE*>(data);

			BYTE stat[256];
			int n = myGetKeyState(stat);

			if (n > 0) {
				gVKeyboard->transToDIKState(stat, n);

				for (int i = 0; i < n; i ++) {
					if (stat[i] & 0x80)
						keys[i] |= 0x80;
				}
			}
		}

		return hr;
	}

	HRESULT WINAPI Mine_Keyboard_GetDeviceData(DWORD size, LPDIDEVICEOBJECTDATA data, 
		LPDWORD numElements, DWORD flags)
	{
		_TRACE(__FUNCTION__);
		size_t n = myGetKeyData(data, *numElements);

		if (n > 0) {

			*numElements = (DWORD )n;

			_TRACE(__FUNCTION__ ": myGetKeyData() processed. numElements = %d", n);
			return NO_ERROR;
		}

		return (this->*Real_IDirectInputDevice8_GetDeviceDataA)(size, data, numElements, flags);
	}

	HRESULT WINAPI Mine_Keyboard_Acquire()
	{
		_TRACE(__FUNCTION__);

		if (myHasData())
			return DI_OK;

		return (this->*Real_IDirectInputDevice8_Acquire)();
	}

	//////////////////////////////////////////////////////////////////////////

	HRESULT WINAPI Mine_Mouse_GetDeviceState(DWORD size, LPVOID data)
	{
		_TRACE(__FUNCTION__);

		return (this->*Real_IDirectInputDevice8_GetDeviceStateA)(size, data);
	}

	HRESULT WINAPI Mine_Mouse_GetDeviceData(DWORD size, LPDIDEVICEOBJECTDATA data, LPDWORD numElements, DWORD flags)
	{
		_TRACE(__FUNCTION__);

		return (this->*Real_IDirectInputDevice8_GetDeviceDataA)(size, data, numElements, flags);
	}

	HRESULT WINAPI Mine_Mouse_Acquire()
	{
		_TRACE(__FUNCTION__);
		return (this->*Real_IDirectInputDevice8_Acquire)();
	}

	//////////////////////////////////////////////////////////////////////////
	
	HRESULT WINAPI Mine_GetDeviceState(DWORD size, LPVOID data)
	{
		_TRACE(__FUNCTION__);

		if (IsKeyboard()) {
			return Mine_Keyboard_GetDeviceState(size, data);
		} else {
			return Mine_Mouse_GetDeviceState(size, data);
		}
	}

	HRESULT WINAPI Mine_GetDeviceData(DWORD size, LPDIDEVICEOBJECTDATA data, LPDWORD numElements, DWORD flags)
	{
		_TRACE(__FUNCTION__);

		if (IsKeyboard()) {
			return Mine_Keyboard_GetDeviceData(size, data, numElements, flags);
		} else {
			return Mine_Mouse_GetDeviceData(size, data, numElements, flags);
		}

	}

	HRESULT WINAPI Mine_Acquire()
	{
		_TRACE(__FUNCTION__);

		if (IsKeyboard()) {
			return Mine_Keyboard_Acquire();
		} else {
			return Mine_Mouse_Acquire();
		}
	}
};

/*
class IDirectInputDevice8DetourW: public IDirectInputDevice8W {
public:

	HRESULT WINAPI Mine_GetDeviceState(DWORD size, LPVOID data)
	{
		HRESULT hr = (this->*Real_IDirectInputDevice8_GetDeviceStateW)(size, data);

		if(SUCCEEDED(hr) && size == sizeof(BYTE) * 256)
		{
			BYTE* keys = static_cast<BYTE*>(data);

			BYTE stat[256];
			int n = myGetKeyState(stat);

			if (n > 0) {
				gVKeyboard->transToDIKState(stat, n);

				for (int i = 0; i < n; i ++) {
					if (stat[i] & 0x80)
						keys[i] |= 0x80;
				}
			}
		}

		return hr;
	}
};
*/

bool HookDInput2()
{
	IDirectInput8A* pDInputA;
	IDirectInputDevice8A* pDeviceA;

	//IDirectInput8W* pDInputW;
	//IDirectInputDevice8W* pDeviceW;

	HRESULT hr = ::DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
		IID_IDirectInput8A, (PVOID* )&pDInputA, NULL);

	if (!SUCCEEDED(hr)) {
		// log error
		return false;
	}	

	hr = pDInputA->CreateDevice(GUID_SysKeyboard, &pDeviceA, NULL);

	if (!SUCCEEDED(hr)) {
		// log error
		return false;
	}

	/*
	hr = ::DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
		IID_IDirectInput8W, (PVOID* )&pDInputW, NULL);

	if (!SUCCEEDED(hr)) {
		// log error
		return false;
	}

	hr = pDInputW->CreateDevice(GUID_SysKeyboard, &pDeviceW, NULL);

	int index = GetVirFuncIndex(&(PVOID &)IDirectInputDevice8A::GetDeviceState, ConvStdCall);

	PVOID fn1 = GetVirFuncAddr(pDeviceA, index);
	PVOID fn2 = GetVirFuncAddr(pDeviceW, index);
	*/
	int index = GetVirFuncIndex(&(PVOID &)IDirectInputDevice8A::GetDeviceState, ConvStdCall);

	bool result = HookVirFunc2(pDeviceA, index, &IDirectInputDevice8DetourA::Mine_GetDeviceState, 
		Real_IDirectInputDevice8_GetDeviceStateA);

	if (result) {

		index = GetVirFuncIndex(&(PVOID &)IDirectInputDevice8A::GetDeviceData, ConvStdCall);

		result = HookVirFunc2(pDeviceA, index, &IDirectInputDevice8DetourA::Mine_GetDeviceData, 
			Real_IDirectInputDevice8_GetDeviceDataA);
	}

	if (result) {

		index = GetVirFuncIndex(&(PVOID &)IDirectInputDevice8A::Acquire, ConvStdCall);

		result = HookVirFunc2(pDeviceA, index, &IDirectInputDevice8DetourA::Mine_Acquire, 
			Real_IDirectInputDevice8_Acquire);
	}

	// HookVirFunc2(pDeviceW, index, &IDirectInputDevice8DetourW::Mine_GetDeviceState, 
	//	Real_IDirectInputDevice8_GetDeviceStateW);

	//CHAR BUF[256];
	//pDeviceA->GetDeviceState(256, BUF);
	//pDInputW->GetDeviceState(0, NULL);

	pDeviceA->Release();
	
	return result;
}
