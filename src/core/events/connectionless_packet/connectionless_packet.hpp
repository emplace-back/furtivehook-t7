#pragma once
#include "dependencies/std_include.hpp"

namespace events::connectionless_packet
{
	size_t cl_dispatch_connectionless_packet_stub();
	void initialize();

	extern bool log_commands;
}