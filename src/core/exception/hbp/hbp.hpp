#pragma once
#include "dependencies/std_include.hpp"

namespace exception::hbp
{
	using callback = std::function<std::int32_t(const LPEXCEPTION_POINTERS&)>; 
	void register_exception(const std::uintptr_t address, const callback & callback);
	
	extern std::unordered_map<std::uintptr_t, callback> exceptions;
}