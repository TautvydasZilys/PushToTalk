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

	bool operator==(const IMMDevice* other) const;

	void Mute();
	void Unmute();
};