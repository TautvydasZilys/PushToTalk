#pragma once

struct CoInitializeWrapper
{
	inline CoInitializeWrapper()
	{
		auto result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		Assert(result == S_OK);
	}

	inline ~CoInitializeWrapper()
	{
		CoUninitialize();
	}
};
