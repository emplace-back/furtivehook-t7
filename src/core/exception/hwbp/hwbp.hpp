#pragma once
#include "dependencies/std_include.hpp"

namespace exception::hwbp
{
	using callback = std::function<void(CONTEXT&)>;

	bool handle_exception(const LPEXCEPTION_POINTERS ex);
	void register_hook(const std::uintptr_t address, const callback & callback);
	void register_hook(const std::uintptr_t address, const std::function<size_t()>& callback);
	void initialize();
}