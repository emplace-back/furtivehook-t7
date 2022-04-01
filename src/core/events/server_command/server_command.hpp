#pragma once
#include "dependencies/std_include.hpp"

namespace events::server_command
{
	bool __fastcall handle_command();
	void initialize();

	extern bool log_commands;
}