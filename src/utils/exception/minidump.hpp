#pragma once
#include "dependencies/std_include.hpp"

namespace utils::exception::minidump
{
	std::string generate_minidump_filename();
	std::string create_minidump(const LPEXCEPTION_POINTERS exceptioninfo);
}