#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <Windows.h>

#include <stdlib.h>

/* C++ Çì´õ */
#include <iostream>
#include <fstream>

#include <filesystem>
#include <source_location>

#include <string>
#include <string_view>

#include <array>
#include <queue>
#include <vector>
#include <functional>

#include <numeric>
#include <algorithm>

#include <ranges>
#include <concepts>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <set>

#include <DirectXMath.h>

#include "external/include/DirectXTK12/SimpleMath.h"

#ifdef _DEBUG 
#pragma comment(lib,"external/lib/debug/DirectXTK12.lib")
#else
#pragma comment(lib,"external/lib/release/DirectXTK12.lib")
#endif

#include "Protocol.h"

extern class Timer gTimer;