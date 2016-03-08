#pragma once

class KeyboardDevice : public IDirectInputDevice8
{
public:
	KeyboardDevice(IDirectInputDevice8* device) : m_device(device)
	{
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj)
	{
		_TRACE( __FUNCTION__ );
		return m_device->QueryInterface(riid, ppvObj);
	}

	STDMETHOD_(ULONG,AddRef)()
	{
		_TRACE( __FUNCTION__ );
		return m_device->AddRef();
	}

	STDMETHOD_(ULONG,Release)()
	{
		_TRACE( __FUNCTION__ );
		ULONG count = m_device->Release();
		if(0 == count)
			delete this;

		return count;
	}

	/*** IDirectInputDevice8 methods ***/
	STDMETHOD(GetCapabilities)(LPDIDEVCAPS devCaps)
	{
		_TRACE( __FUNCTION__ );
		return m_device->GetCapabilities(devCaps);
	}

	STDMETHOD(EnumObjects)(LPDIENUMDEVICEOBJECTSCALLBACK callback, LPVOID ref, DWORD flags)	
	{
		_TRACE( __FUNCTION__ );
		return m_device->EnumObjects(callback, ref, flags);
	}

	STDMETHOD(GetProperty)(REFGUID rguid, LPDIPROPHEADER ph)
	{
		_TRACE( __FUNCTION__ );
		return m_device->GetProperty(rguid, ph);
	}

	STDMETHOD(SetProperty)(REFGUID rguid, LPCDIPROPHEADER ph)
	{
		_TRACE( __FUNCTION__ );
		return m_device->SetProperty(rguid, ph);

		/*
		You can detect immediate/buffered modes as such:
		
		HRESULT hr = m_device->SetProperty(rguid, ph);

		if(SUCCEEDED(hr) && rguid == DIPROP_BUFFERSIZE)
		{
			DWORD data = *reinterpret_cast<const DWORD*>(ph + 1);
			m_is_immediate = (data == 0);
		}

		return hr;
		*/
	}

	STDMETHOD(Acquire)()
	{
		_TRACE( __FUNCTION__ );
		return m_device->Acquire();
	}

	STDMETHOD(Unacquire)()
	{
		_TRACE( __FUNCTION__ );
		return m_device->Unacquire();
	}

	STDMETHOD(GetDeviceState)(DWORD size, LPVOID data);
	STDMETHOD(GetDeviceData)(DWORD size, LPDIDEVICEOBJECTDATA data, LPDWORD numElements, DWORD flags);

	STDMETHOD(SetDataFormat)(LPCDIDATAFORMAT df)
	{
		_TRACE( __FUNCTION__ );
		return m_device->SetDataFormat(df);
	}

	STDMETHOD(SetEventNotification)(HANDLE event)
	{
		_TRACE( __FUNCTION__ );
		return m_device->SetEventNotification(event);
	}

	STDMETHOD(SetCooperativeLevel)(HWND window, DWORD level)
	{
		_TRACE( __FUNCTION__ );
		return m_device->SetCooperativeLevel(window, level);
	}

	STDMETHOD(GetObjectInfo)(LPDIDEVICEOBJECTINSTANCE object, DWORD objId, DWORD objHow)
	{
		_TRACE( __FUNCTION__ );
		return m_device->GetObjectInfo(object, objId, objHow);
	}

	STDMETHOD(GetDeviceInfo)(LPDIDEVICEINSTANCE di)
	{
		_TRACE( __FUNCTION__ );
		return m_device->GetDeviceInfo(di);
	}

	STDMETHOD(RunControlPanel)(HWND owner, DWORD flags)
	{
		_TRACE( __FUNCTION__ );
		return m_device->RunControlPanel(owner, flags);
	}

	STDMETHOD(Initialize)(HINSTANCE instance, DWORD version, REFGUID rguid)
	{
		_TRACE( __FUNCTION__ );
		return m_device->Initialize(instance, version, rguid);
	}

	STDMETHOD(CreateEffect)(REFGUID rguid, LPCDIEFFECT effect_params, LPDIRECTINPUTEFFECT* effect, LPUNKNOWN unknown)
	{
		_TRACE( __FUNCTION__ );
		return m_device->CreateEffect(rguid, effect_params, effect, unknown);
	}

    STDMETHOD(EnumEffects)(LPDIENUMEFFECTSCALLBACK callback, LPVOID ref, DWORD type)
	{
		_TRACE( __FUNCTION__ );
		return m_device->EnumEffects(callback, ref, type);
	}

    STDMETHOD(GetEffectInfo)(LPDIEFFECTINFO effect_info, REFGUID rguid)
	{
		_TRACE( __FUNCTION__ );
		return m_device->GetEffectInfo(effect_info, rguid);
	}

    STDMETHOD(GetForceFeedbackState)(LPDWORD state)
	{
		_TRACE( __FUNCTION__ );
		return m_device->GetForceFeedbackState(state);
	}

    STDMETHOD(SendForceFeedbackCommand)(DWORD flags)
	{
		_TRACE( __FUNCTION__ );
		return m_device->SendForceFeedbackCommand(flags);
	}

    STDMETHOD(EnumCreatedEffectObjects)(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK callback, LPVOID ref, DWORD flags)
	{
		_TRACE( __FUNCTION__ );
		return m_device->EnumCreatedEffectObjects(callback, ref, flags);
	}

    STDMETHOD(Escape)(LPDIEFFESCAPE escape)
	{
		_TRACE( __FUNCTION__ );
		return m_device->Escape(escape);
	}

    STDMETHOD(Poll)()
	{
		_TRACE( __FUNCTION__ );
		return m_device->Poll();
	}

	STDMETHOD(SendDeviceData)(DWORD size, LPCDIDEVICEOBJECTDATA data, LPDWORD num_elements, DWORD flags)
	{
		_TRACE( __FUNCTION__ );
		return m_device->SendDeviceData(size, data, num_elements, flags);
	}

	STDMETHOD(EnumEffectsInFile)(LPCTSTR file_name, LPDIENUMEFFECTSINFILECALLBACK callback, LPVOID ref, DWORD flags)
	{
		_TRACE( __FUNCTION__ );
		return m_device->EnumEffectsInFile(file_name, callback, ref, flags);
	}

    STDMETHOD(WriteEffectToFile)(LPCTSTR file_name, DWORD num_entries, LPDIFILEEFFECT effects, DWORD flags)
	{
		_TRACE( __FUNCTION__ );
		return m_device->WriteEffectToFile(file_name, num_entries, effects, flags);
	}

    STDMETHOD(BuildActionMap)(LPDIACTIONFORMAT format, LPCTSTR username, DWORD flags)
	{
		_TRACE( __FUNCTION__ );
		return m_device->BuildActionMap(format, username, flags);
	}

    STDMETHOD(SetActionMap)(LPDIACTIONFORMAT format, LPCTSTR username, DWORD flags)
	{
		_TRACE( __FUNCTION__ );
		return m_device->SetActionMap(format, username, flags);
	}

    STDMETHOD(GetImageInfo)(LPDIDEVICEIMAGEINFOHEADER image_header)
	{
		_TRACE( __FUNCTION__ );
		return m_device->GetImageInfo(image_header);
	}

private:
	IDirectInputDevice8* m_device;	
};