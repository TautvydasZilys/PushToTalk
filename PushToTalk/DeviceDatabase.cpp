#include "PrecompiledHeader.h"
#include "DeviceDatabase.h"
#include "DeviceSentry.h"

const int kScanPeriodInMilliseconds = 2000;

DeviceDatabase::DeviceDatabase()
{
	DeviceSentry::Start(kScanPeriodInMilliseconds, [this](const std::vector<IMMDevice*>& devices)
	{
		UpdateDevices(devices);
	});
}

DeviceDatabase::~DeviceDatabase()
{
}

void DeviceDatabase::UpdateDevices(const std::vector<IMMDevice*>& devices)
{
	CriticalSection::ScopedLock lock(m_CriticalSection);

	for (auto i = 0u; i < m_Devices.size(); i++)
	{
		auto& myDevice = m_Devices[i];
		
		if (std::find_if(devices.begin(), devices.end(), [&myDevice](const IMMDevice* device) { return myDevice == device; }) == devices.end())
		{
			m_Devices[i] = std::move(m_Devices[m_Devices.size() - 1]);
			m_Devices.pop_back();
			i--;
		}
	}

	for (auto& device : devices)
	{
		if (std::find(m_Devices.begin(), m_Devices.end(), device) == m_Devices.end())
		{
			m_Devices.emplace_back(device);
		}
	}	
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