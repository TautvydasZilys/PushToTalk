template <typename Callback>
inline static void DeviceSentry::Start(const int scanPeriodInMilliseconds, Callback&& callback)
{
	Assert(s_Instance == nullptr);

	s_Instance = std::unique_ptr<DeviceSentry>(new DeviceSentry(scanPeriodInMilliseconds));
	s_Instance->m_Running = true;
	s_Instance->PerformLoop(callback);

	s_Instance->m_SentryThread = std::thread([callback]
	{
		s_Instance->Run(callback);
	});
}

template <typename Callback>
inline void DeviceSentry::Run(Callback&& callback)
{
	Sleep(m_ScanPeriodInMilliseconds);

	while (m_Running)
	{
		PerformLoop(callback);
		Sleep(m_ScanPeriodInMilliseconds);
	}
}

template <typename Callback>
inline void DeviceSentry::PerformLoop(Callback&& callback)
{
	ComPtr<IMMDeviceCollection> deviceCollection;
	UINT deviceCount;

	auto result = m_DeviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &deviceCollection);
	Assert(result == S_OK);

	result = deviceCollection->GetCount(&deviceCount);
	Assert(result == S_OK);

	std::vector<IMMDevice*> devices(deviceCount);

	for (auto i = 0u; i < deviceCount; i++)
	{
		result = deviceCollection->Item(i, &devices[i]);
		Assert(result == S_OK);
	}

	callback(devices);

	for (auto& device : devices)
	{
		device->Release();
	}
}