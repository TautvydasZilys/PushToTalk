#pragma once

class Device
{
	ComPtr<IMMDevice> m_Device;
	ComPtr<IAudioEndpointVolume> m_EndpointVolume;

public:
	Device(IMMDevice* device);
	Device(const Device&) = delete;
	Device(Device&& other);

	Device& operator=(const Device&) = delete;
	Device& operator=(Device&&);

	bool operator==(IMMDevice* other) const;
	bool operator==(const wchar_t* otherDeviceId) const;

	void Mute();
	void Unmute();
};