#include "PrecompiledHeader.h"
#include "DeviceSentry.h"

std::unique_ptr<DeviceSentry> DeviceSentry::s_Instance;

DeviceSentry::DeviceSentry(const int scanPeriodInMilliseconds) :
	m_ScanPeriodInMilliseconds(scanPeriodInMilliseconds),
	m_Running(false)
{
	auto result = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), &m_DeviceEnumerator);
	Assert(result == S_OK);

}

void DeviceSentry::Stop()
{
	Assert(s_Instance != nullptr);
	Assert(s_Instance->m_Running);

	s_Instance->m_Running = false;
	s_Instance->m_SentryThread.join();
	s_Instance = nullptr;
}