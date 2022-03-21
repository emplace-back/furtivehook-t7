#pragma once
#include "dependencies/std_include.hpp"

namespace utils::exception::minidump
{
	bool write(const LPEXCEPTION_POINTERS ex);
}