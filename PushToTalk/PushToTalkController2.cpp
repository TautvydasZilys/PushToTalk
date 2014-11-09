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
	windowInfo.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowInfo.hInstance = GetModuleHandle(nullptr);
	windowInfo.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	windowInfo.hIconSm = windowInfo.hIcon;
	windowInfo.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowInfo.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowInfo.lpszMenuName = NULL;
	windowInfo.lpszClassName = kProgramName;
	windowInfo.cbSize = sizeof(WNDCLASSEX);

	windowInfo.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT
	{
		return s_Instance->WindowProc(hWnd, uMsg, wParam, lParam);
	};

	RegisterClassEx(&windowInfo);

	m_WindowHandle = CreateWindow(windowInfo.lpszClassName, windowInfo.lpszClassName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		0, 0, 1, 1, nullptr, nullptr, windowInfo.hInstance, this);
	Assert(m_WindowHandle != nullptr);

	RAWINPUTDEVICE rawInputDevice;
	
	// http://www.usb.org/developers/hidpage/Hut1_12v2.pdf
	rawInputDevice.usUsagePage = 0x01;
	rawInputDevice.usUsage = 0x06;
	rawInputDevice.dwFlags = RIDEV_INPUTSINK;
	rawInputDevice.hwndTarget = m_WindowHandle;

	auto result = RegisterRawInputDevices(&rawInputDevice, 1, sizeof(rawInputDevice));
	Assert(result != 0);

	m_DeviceDatabase.MuteAllDevices();
	m_Muted = true;
}

PushToTalkController::~PushToTalkController()
{
	DestroyWindow(m_WindowHandle);
	m_WindowHandle = nullptr;

	UnregisterClass(kProgramName, GetModuleHandle(nullptr));

	Assert(s_Instance != nullptr);
	s_Instance = nullptr;
}

void PushToTalkController::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	for (;;)
	{
		auto result = GetMessage(&msg, m_WindowHandle, 0, 0);

		if (result != 0)
		{
			Assert(result != -1);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			break;
		}
	}
}

LRESULT PushToTalkController::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
	case WM_QUIT:
	case WM_CLOSE:
		return 0;

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
