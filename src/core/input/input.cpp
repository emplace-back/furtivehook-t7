#include "dependencies/std_include.hpp"
#include "input.hpp"

namespace input
{
	WNDPROC wndproc_;
	std::vector<hotkey_t> registered_keys; 

	bool should_ignore_msg(UINT msg)
	{
		switch (msg)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_SETCURSOR:
		case WM_MOUSEACTIVATE:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MOUSEMOVE:
		case WM_NCHITTEST:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHOVER:
		case WM_ACTIVATEAPP:
			return true;
		default:
			return false;
		}
	}

	bool is_key_down(UINT key, UINT msg, WPARAM wparam)
	{
		return msg == WM_KEYDOWN && wparam == key;
	}

	bool was_key_pressed(UINT key, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		return is_key_down(key, msg, wparam) && !(lparam >> 0x1E);
	}

	long __stdcall wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		auto should_ignore = menu::is_open() && !ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

		for (const auto& hotkey : registered_keys)
		{
			if (is_key_down(hotkey.key, msg, wparam) && hotkey.block)
			{
				should_ignore = true;
			}

			if (was_key_pressed(hotkey.key, msg, wparam, lparam))
			{
				hotkey.func();

				if (hotkey.block)
				{
					should_ignore = true;
				}
			}
		}

		if (should_ignore && should_ignore_msg(msg))
		{
			return true;
		}

		return CallWindowProcA(wndproc_, hwnd, msg, wparam, lparam);
	}

	void on_key(UINT key, void(*cb)(), bool block)
	{
		registered_keys.emplace_back(hotkey_t{ key, cb, block });
	}

	void initialize(HWND hwnd)
	{
		wndproc_ = WNDPROC(SetWindowLongPtr(hwnd, GWLP_WNDPROC, LONG_PTR(wndproc)));
	}
}