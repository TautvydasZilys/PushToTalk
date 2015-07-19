#include "PrecompiledHeader.h"
#include "DeviceDatabase.h"

DeviceDatabase::DeviceDatabase() :
	m_RefCount(1)
{
	auto hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), &m_DeviceEnumerator);
	Assert(SUCCEEDED(hr));

	hr = m_DeviceEnumerator->RegisterEndpointNotificationCallback(this);
	Assert(SUCCEEDED(hr));

	EnumerateDevices();
}

DeviceDatabase::~DeviceDatabase()
{
	auto hr = m_DeviceEnumerator->UnregisterEndpointNotificationCallback(this);
	Assert(SUCCEEDED(hr));
}

void DeviceDatabase::EnumerateDevices()
{
	CriticalSection::ScopedLock lock(m_CriticalSection);

	ComPtr<IMMDeviceCollection> deviceCollection;
	UINT deviceCount;

	auto hr = m_DeviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &deviceCollection);
	Assert(SUCCEEDED(hr));

	hr = deviceCollection->GetCount(&deviceCount);
	Assert(SUCCEEDED(hr));

	for (auto i = 0u; i < deviceCount; i++)
	{
		ComPtr<IMMDevice> device;
		hr = deviceCollection->Item(i, &device);
		Assert(SUCCEEDED(hr));

		if (std::find(m_Devices.begin(), m_Devices.end(), device.Get()) == m_Devices.end())
			m_Devices.emplace_back(device.Get());
	}
}

HRESULT STDMETHODCALLTYPE DeviceDatabase::QueryInterface(REFIID riid, void** ppvObject)
{
	if (__uuidof(IUnknown) == riid)
	{
		*ppvObject = static_cast<IUnknown*>(this);
		return S_OK;
	}

	if (__uuidof(IMMNotificationClient) == riid)
	{
		*ppvObject = static_cast<IMMNotificationClient*>(this);
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE DeviceDatabase::AddRef()
{
	return InterlockedIncrement(&m_RefCount);
}

ULONG STDMETHODCALLTYPE DeviceDatabase::Release()
{
	return InterlockedDecrement(&m_RefCount);
}

static ComPtr<IMMDevice> GetDeviceFromId(IMMDeviceEnumerator* deviceEnumerator, LPCWSTR pwstrDeviceId)
{
	ComPtr<IMMDevice> device;
	auto hr = deviceEnumerator->GetDevice(pwstrDeviceId, &device);
	Assert(SUCCEEDED(hr));

	return device;
}

static EDataFlow GetDeviceDataFlow(IMMDevice* device)
{
	ComPtr<IMMEndpoint> endpoint;
	auto hr = device->QueryInterface<IMMEndpoint>(&endpoint);
	Assert(SUCCEEDED(hr));

	EDataFlow dataFlow;
	hr = endpoint->GetDataFlow(&dataFlow);
	Assert(SUCCEEDED(hr));

	return dataFlow;
}

HRESULT STDMETHODCALLTYPE DeviceDatabase::OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState)
{
	auto device = GetDeviceFromId(m_DeviceEnumerator.Get(), pwstrDeviceId);
	auto dataFlow = GetDeviceDataFlow(device.Get());

	if (dataFlow == eCapture)
	{
		CriticalSection::ScopedLock lock(m_CriticalSection);

		if (dwNewState == DEVICE_STATE_ACTIVE)
		{
			if (std::find(m_Devices.begin(), m_Devices.end(), device.Get()) == m_Devices.end())
				m_Devices.emplace_back(device.Get());
		}
		else
		{
			auto deviceIt = std::find(m_Devices.begin(), m_Devices.end(), device.Get());

			if (deviceIt != m_Devices.end())
				m_Devices.erase(deviceIt);
		}
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DeviceDatabase::OnDeviceAdded(LPCWSTR pwstrDeviceId)
{
	auto device = GetDeviceFromId(m_DeviceEnumerator.Get(), pwstrDeviceId);
	auto dataFlow = GetDeviceDataFlow(device.Get());

	if (dataFlow == eCapture)
	{
		CriticalSection::ScopedLock lock(m_CriticalSection);

		if (std::find(m_Devices.begin(), m_Devices.end(), device.Get()) == m_Devices.end())
			m_Devices.emplace_back(device.Get());
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DeviceDatabase::OnDeviceRemoved(LPCWSTR pwstrDeviceId)
{
	CriticalSection::ScopedLock lock(m_CriticalSection);

	auto deviceIt = std::find(m_Devices.begin(), m_Devices.end(), pwstrDeviceId);

	if (deviceIt != m_Devices.end())
		m_Devices.erase(deviceIt);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE DeviceDatabase::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId)
{
	// Nothing to do
	return S_OK;
}

HRESULT STDMETHODCALLTYPE DeviceDatabase::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key)
{
	// Nothing to do
	return S_OK;
}

void DeviceDatabase::MuteAllDevices()
{
	CriticalSection::ScopedLock lock(m_CriticalSection);

	for (auto& device : m_Devices)
	{
		device.Mute();
	}
}

void DeviceDatabase::UnmuteAllDevices()
{
	CriticalSection::ScopedLock lock(m_CriticalSection);

	for (auto& device : m_Devices)
	{
		device.Unmute();
	}
}