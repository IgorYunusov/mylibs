#include "mylib.h"
#include "KeyboardDevice.h"
#include "Utils/VInputDevice.h"

extern VKeyboard* gVKeyboard;

int myGetKeyState(BYTE* stat);

HRESULT KeyboardDevice::GetDeviceState(DWORD size, LPVOID data)
{
	_TRACE( __FUNCTION__ );

	HRESULT hr = m_device->GetDeviceState(size, data);

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

HRESULT KeyboardDevice::GetDeviceData(DWORD size, LPDIDEVICEOBJECTDATA data, LPDWORD numElements, DWORD flags)
{
	_TRACE( __FUNCTION__ );
	return m_device->GetDeviceData(size, data, numElements, flags);
}
