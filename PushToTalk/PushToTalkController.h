#pragma once

#include "DeviceDatabase.h"

class PushToTalkController
{
private:
	DeviceDatabase m_DeviceDatabase;
	HWND m_WindowHandle;
	bool m_Muted;

	LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleInput(WPARAM wParam, LPARAM lParam);

public:
	PushToTalkController();
	~PushToTalkController();

	void Run();
};

