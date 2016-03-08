#pragma once

#include "KeyboardDevice.h"

class MyDirectInput8 : public IDirectInput8
{
public:
	MyDirectInput8(IDirectInput8* di) : m_di(di)
	{
	}

	/*** IUnknown methods ***/
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObj)
	{
		return m_di->QueryInterface(riid, ppvObj);
	}

    ULONG STDMETHODCALLTYPE AddRef()
	{
		return m_di->AddRef();
	}

    ULONG STDMETHODCALLTYPE Release()
	{
		ULONG count = m_di->Release();
		if(0 == count)
			delete this;

		return count;
	}

    /*** IDirectInput8 methods ***/
    STDMETHOD(CreateDevice)(REFGUID rguid, IDirectInputDevice8** device, LPUNKNOWN unknown)
	{
		HRESULT hr = m_di->CreateDevice(rguid, device, unknown);
		if(SUCCEEDED(hr) && rguid == GUID_SysKeyboard)
		{
			// Return our own keyboard device that checks for injected keypresses
			*device = new KeyboardDevice(*device);
		}

		return hr;
	}

    STDMETHOD(EnumDevices)(DWORD devType,LPDIENUMDEVICESCALLBACK callback, LPVOID ref, DWORD flags)
	{
		return m_di->EnumDevices(devType, callback, ref, flags);
	}

    STDMETHOD(GetDeviceStatus)(REFGUID rguid)
	{
		return m_di->GetDeviceStatus(rguid);
	}

    STDMETHOD(RunControlPanel)(HWND owner, DWORD flags)
	{
		return m_di->RunControlPanel(owner, flags);
	}

    STDMETHOD(Initialize)(HINSTANCE instance, DWORD version)
	{
		return m_di->Initialize(instance, version);
	}

    STDMETHOD(FindDevice)(REFGUID rguid, LPCTSTR name, LPGUID guidInstance)
	{
		return m_di->FindDevice(rguid, name, guidInstance);
	}

    STDMETHOD(EnumDevicesBySemantics)(LPCTSTR username, LPDIACTIONFORMAT action,
		LPDIENUMDEVICESBYSEMANTICSCB callback, LPVOID ref, DWORD flags)
	{
		return m_di->EnumDevicesBySemantics(username, action, callback, ref, flags);
	}

    STDMETHOD(ConfigureDevices)(LPDICONFIGUREDEVICESCALLBACK callback, LPDICONFIGUREDEVICESPARAMS params,
		DWORD flags, LPVOID ref)
	{
		return m_di->ConfigureDevices(callback, params, flags, ref);
	}

private:
	IDirectInput8* m_di;
};
