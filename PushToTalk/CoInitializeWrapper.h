#pragma once

struct CoInitializeWrapper
{
	inline CoInitializeWrapper()
	{
		auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		Assert(SUCCEEDED(hr));
	}

	inline ~CoInitializeWrapper()
	{
		CoUninitialize();
	}
};
