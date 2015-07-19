#include "PrecompiledHeader.h"
#include "Device.h"

Device::Device(IMMDevice* device) :
	m_Device(device)
{
	auto hr = m_Device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, &m_EndpointVolume);
	Assert(SUCCEEDED(hr));
}

Device::Device(Device&& other) :
	m_Device(std::move(other.m_Device)),
	m_EndpointVolume(std::move(other.m_EndpointVolume))
{
}

Device& Device::operator=(Device&& other)
{
	m_Device = std::move(other.m_Device);
	m_EndpointVolume = std::move(other.m_EndpointVolume);
	return *this;
}

bool Device::operator==(IMMDevice* other) const
{
	wchar_t* otherDeviceId;
	auto hr = other->GetId(&otherDeviceId);
	Assert(SUCCEEDED(hr));

	auto result = *this == otherDeviceId;
	CoTaskMemFree(otherDeviceId);
	return result;
}

bool Device::operator==(const wchar_t* otherDeviceId) const
{
	wchar_t* myDeviceId;
	auto hr = m_Device->GetId(&myDeviceId);
	Assert(SUCCEEDED(hr));

	auto result = wcscmp(myDeviceId, otherDeviceId) == 0;
	CoTaskMemFree(myDeviceId);
	return result;
}

void Device::Mute()
{
	auto hr = m_EndpointVolume->SetMute(TRUE, nullptr);
	Assert(SUCCEEDED(hr));
}

void Device::Unmute()
{
	auto hr = m_EndpointVolume->SetMute(FALSE, nullptr);
	Assert(SUCCEEDED(hr));
}
