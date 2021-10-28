#pragma once
#include "dependencies/std_include.hpp"

IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct hotkey_t
{
	UINT key;
	void(*func)();
	bool block;
};

namespace input
{
	void initialize(HWND hwnd);
	void on_key(UINT key, void(*cbb)(), bool block = false);
}