#pragma once
#include "dependencies/std_include.hpp"

namespace exception::pageguard
{
	using callback = std::function<void()>;

	bool handle_exception(const LPEXCEPTION_POINTERS ex);
	void page_guard_address(const std::uintptr_t address);
	void initialize();
}