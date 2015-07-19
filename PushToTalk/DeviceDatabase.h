#pragma once

#include "CriticalSection.h"
#include "Device.h"

class DeviceDatabase :
	private IMMNotificationClient
{
private:
	ComPtr<IMMDeviceEnumerator> m_DeviceEnumerator;
	std::vector<Device> m_Devices;
	CriticalSection m_CriticalSection;

	void EnumerateDevices();

	ULONG m_RefCount;	// Doesn't manage lifetime of this object, because it's not publicly a COM object

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;
	virtual ULONG STDMETHODCALLTYPE AddRef() override;
	virtual ULONG STDMETHODCALLTYPE Release() override;
	virtual HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) override;
	virtual HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId) override;
	virtual HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId) override;
	virtual HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) override;
	virtual HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) override;

public:
	DeviceDatabase();
	~DeviceDatabase();

	DeviceDatabase(const DeviceDatabase&) = delete;
	DeviceDatabase& operator=(const DeviceDatabase&) = delete;

	void MuteAllDevices();
	void UnmuteAllDevices();
};

