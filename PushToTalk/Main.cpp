#include "PrecompiledHeader.h"
#include "CoInitializeWrapper.h"
#include "PushToTalkController.h"

inline bool IsElevated()
{
	bool isElevated = false;
	HANDLE hToken = NULL;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		TOKEN_ELEVATION elevation;
		DWORD cbSize = sizeof(elevation);

		if (GetTokenInformation(hToken, TokenElevation, &elevation, cbSize, &cbSize))
			isElevated = elevation.TokenIsElevated != 0;

		CloseHandle(hToken);
	}

	return isElevated;
}

inline bool ElevateByForking()
{
	if (IsElevated())	// We're already elevated, nothing to do
		return true;

	const uint32_t bufferSize = MAX_PATH + 1;
	wchar_t executable[bufferSize];

	auto executableLength = GetModuleFileNameW(nullptr, executable, bufferSize);
	Assert(executableLength != 0);

	SHELLEXECUTEINFOW shellExecuteInfo;
	ZeroMemory(&shellExecuteInfo, sizeof(shellExecuteInfo));

	shellExecuteInfo.cbSize = sizeof(shellExecuteInfo);
	shellExecuteInfo.lpVerb = L"runas";
	shellExecuteInfo.lpFile = executable;
	shellExecuteInfo.nShow = SW_NORMAL;

	if (ShellExecuteExW(&shellExecuteInfo))
	{
		// Succeeded executing elevated process. Return false to signify that this instance should quit.
		return false;
	}

	// We failed to elevate ourselves. That means that we can still function, even though push to talk will not
	// work if there is a window of an elevated process in foreground. Return true to signify that we should continue execution.
	return true;
}

int main()
{
	if (!ElevateByForking())
		return 0;

	CoInitializeWrapper coInit;
	PushToTalkController pushToTalk;
	pushToTalk.Run();

	return 0;
}