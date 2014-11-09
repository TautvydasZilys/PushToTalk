#include "PrecompiledHeader.h"
#include "PushToTalkController.h"

const wchar_t kProgramName[] = L"Push to talk";
const USHORT kPushToTalkKey = VK_CAPITAL;

static PushToTalkController* s_Instance = nullptr;

PushToTalkController::PushToTalkController()
{
	Assert(s_Instance == nullptr);
	s_Instance = this;

	WNDCLASSEX windowInfo;
	ZeroMemory(&windowInfo, sizeof(WNDCLASSEX));

	windowInfo.hInstance = GetModuleHandle(nullptr);
	windowInfo.lpszClassName = kProgramName;
	windowInfo.cbSize = sizeof(WNDCLASSEX);
	windowInfo.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT
	{
		return s_Instance->WindowProc(hWnd, uMsg, wParam, lParam);
	};

	RegisterClassEx(&windowInfo);

	m_WindowHandle = CreateWindowEx(0, windowInfo.lpszClassName, windowInfo.lpszClassName, 0, 0, 0, 0, 0, 0, nullptr, nullptr, nullptr);
	Assert(m_WindowHandle != nullptr);
	
	RAWINPUTDEVICE rawInputDevice;
	
	// http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
	rawInputDevice.usUsagePage = 0x01;
	rawInputDevice.usUsage = 0x06;
	rawInputDevice.dwFlags = RIDEV_INPUTSINK;
	rawInputDevice.hwndTarget = m_WindowHandle;

	auto result = RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice));
	Assert(result != FALSE);

	m_DeviceDatabase.MuteAllDevices();
	m_Muted = true;
}

PushToTalkController::~PushToTalkController()
{
	DestroyWindow(m_WindowHandle);
	m_WindowHandle = nullptr;

	UnregisterClass(kProgramName, GetModuleHandle(nullptr));

	m_DeviceDatabase.UnmuteAllDevices();

	Assert(s_Instance != nullptr);
	s_Instance = nullptr;
}

void PushToTalkController::Run()
{
	MSG msg;
	BOOL getMessageResult;
	
	while ((getMessageResult = GetMessage(&msg, nullptr, 0, 0)) != 0)
	{
		Assert(getMessageResult != -1);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT PushToTalkController::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INPUT:
		return HandleInput(wParam, lParam);

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT PushToTalkController::HandleInput(WPARAM wParam, LPARAM lParam)
{
	RAWINPUT rawInput;
	UINT bufferSize = sizeof(rawInput);

	auto result = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawInput, &bufferSize, sizeof(RAWINPUTHEADER));
	Assert(result == sizeof(RAWINPUTHEADER) + sizeof(RAWKEYBOARD));
	
	if (rawInput.header.dwType == RIM_TYPEKEYBOARD && rawInput.data.keyboard.VKey == kPushToTalkKey)
	{
		if (rawInput.data.keyboard.Message == WM_KEYDOWN)
		{
			if (m_Muted)
			{
				m_DeviceDatabase.UnmuteAllDevices();
				m_Muted = false;
			}
		}
		else if (rawInput.data.keyboard.Message == WM_KEYUP)
		{
			if (!m_Muted)
			{
				m_DeviceDatabase.MuteAllDevices();
				m_Muted = true;
			}
		}

		return 0;
	}

	auto rawInputPtr = &rawInput;
	return DefRawInputProc(&rawInputPtr, 1, sizeof(RAWINPUTHEADER));
}
