#pragma once
#include "dependencies/std_include.hpp"

namespace utils::exception::minidump
{
	void write_minidump(const LPEXCEPTION_POINTERS ex);
}