#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <Windows.h>
#include <Wrl.h>

using namespace Microsoft::WRL;

#if _DEBUG
#define Assert(x) do { if (!(x)) { __debugbreak(); } } while (false)
#else
#define Assert(x)
#endif