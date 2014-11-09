#pragma once

#include "CriticalSection.h"

class DeviceSentry
{
	static std::unique_ptr<DeviceSentry> s_Instance;

	const int m_ScanPeriodInMilliseconds;
	ComPtr<IMMDeviceEnumerator> m_DeviceEnumerator;
	std::thread m_SentryThread;
	bool m_Running;

	DeviceSentry(const int scanPeriodInMilliseconds);

	template <typename Callback>
	inline void Run(Callback&& callback);

	template <typename Callback>
	inline void PerformLoop(Callback&& callback);
	
public:
	template <typename Callback>
	inline static void Start(const int scanPeriodInMilliseconds, Callback&& callback);

	static void Stop();
};

#include "DeviceSentry.inl"