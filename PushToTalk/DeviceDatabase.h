#pragma once

#include "CriticalSection.h"
#include "Device.h"

class DeviceDatabase
{
private:
	std::vector<Device> m_Devices;
	CriticalSection m_CriticalSection;

	void UpdateDevices(const std::vector<IMMDevice*>& devices);

public:
	DeviceDatabase();
	~DeviceDatabase();

	void MuteAllDevices();
	void UnmuteAllDevices();
};

