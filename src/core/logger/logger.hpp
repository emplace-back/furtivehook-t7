#pragma once
#include "dependencies/std_include.hpp"

namespace logger
{
	void add_to_console(const char * msg, ...);
	void print_log(const char* msg, ...);
	void print_sv_log(const char * msg, ...);
}