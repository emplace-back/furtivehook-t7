#pragma once

#define no_init_all deprecated

#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <intrin.h>
#include <algorithm>
#include <WinInet.h>
#include <time.h>
#include <dxgi.h>
#include <random>
#include <numeric>
#include <future>
#include <mutex>
#include <regex>
#include <cmath>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <array>
#include <codecvt>
#include <psapi.h>
#include <Shlwapi.h>
#include <dbghelp.h>
#include <filesystem>
#include <shellapi.h>
#include <cctype>
#include <csetjmp>
#include <sstream>
#include <TlHelp32.h>
#include <winternl.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "ntdll.lib")

#include <utils/asmjit/core/jitruntime.h>
#include <utils/asmjit/x86/x86assembler.h>
#include "../thirdparty/nlohmann/json.hpp"
#include "../thirdparty/minhook/MinHook.h"
#include "../thirdparty/steam_api/isteamuser.hpp"
#include "../thirdparty/steam_api/isteamapps.hpp"
#include "../thirdparty/steam_api/isteamfriends.hpp"
#include "../thirdparty/steam_api/isteammatchmaking.hpp"
#include "../thirdparty/steam_api/isteamnetworking.hpp"
#include "../thirdparty/imgui/imgui.h"
#include "../thirdparty/imgui/imgui_internal.h"
#include "../thirdparty/imgui/imgui_impl_dx11.h"
#include "../thirdparty/imgui/imgui_impl_win32.h"

#define LOG_PREFIX "[furtivehook]"

/*
	#ifndef NDEBUG
	#define DEBUG_LOG(__FMT__, ...) utils::print_log(LOG_PREFIX "[" __FUNCTION__ "] " __FMT__ "\n", __VA_ARGS__)
	#endif
*/

#define DEBUG_LOG(__FMT__, ...) utils::print_log(LOG_PREFIX "[" __FUNCTION__ "] " __FMT__ "\n", __VA_ARGS__)

#define PRINT_LOG(__FMT__, ...)	utils::print_log(__FMT__ "\n", __VA_ARGS__)	

#define PRINT_MESSAGE(title, __FMT__, ...) PRINT_LOG(__FMT__, __VA_ARGS__); utils::toast::add_toast(title, utils::string::va(__FMT__, __VA_ARGS__))

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

using namespace std::literals;
using namespace std::chrono_literals;
using json = nlohmann::json;

#include "game/game.hpp"
#include "math/math.hpp"

#include "utils/utils.hpp"
#include "utils/signature.hpp"
#include "utils/io.hpp"
#include "utils/hook.hpp"
#include "utils/memory.hpp"
#include "utils/string.hpp"
#include "utils/concurrency.hpp"
#include "utils/hwbp.hpp"
#include "utils/nt.hpp"
#include "utils/toast.hpp"
#include "utils/thread.hpp"
#include "utils/exception/minidump.hpp"
#include "utils/spoof_call/spoof_call.hpp"

#include "core/misc/misc.hpp"
#include "core/input/input.hpp"
#include "core/rendering/dx.hpp"
#include "core/rendering/rendering.hpp"
#include "core/esp/esp.hpp"
#include "core/menu/menu.hpp"
#include "core/events/events.hpp"
#include "core/nospread/nospread.hpp"
#include "core/events/instant_message/instant_message.hpp"
#include "core/friends/friends.hpp"
#include "core/scheduler/scheduler.hpp"
#include "core/arxan/arxan.hpp"
#include "core/security/security.hpp"
#include "core/session/session.hpp"
#include "core/command/command.hpp"
#include "core/exploit/exploit.hpp"
#include "core/aimbot/aimbot.hpp"
#include "core/aimbot/autowall.hpp"
#include "core/rendering/rendering.hpp"
#include "core/exception/exception.hpp"
#include "core/events/server_command/server_command.hpp"
#include "core/events/connectionless_packet/connectionless_packet.hpp"
#include "core/events/lobby_msg/lobby_msg.hpp"

#include "steam/steam.hpp"