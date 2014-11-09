#include "PrecompiledHeader.h"
#include "Device.h"

Device::Device(IMMDevice* device) :
	m_Device(device)
{
	auto result = m_Device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, &m_EndpointVolume);
	Assert(result == S_OK);	
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

bool Device::operator==(const IMMDevice* other) const
{
	return m_Device.Get() == other;
}

void Device::Mute()
{
	auto result = m_EndpointVolume->SetMute(TRUE, nullptr);
	Assert(result == S_OK || result == S_FALSE);
}

void Device::Unmute()
{
	auto result = m_EndpointVolume->SetMute(FALSE, nullptr);
	Assert(result == S_OK || result == S_FALSE);
}
