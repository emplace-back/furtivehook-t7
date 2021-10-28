#pragma once
#include "dependencies/std_include.hpp"

namespace events::connectionless_packet
{
	using callback = std::function<bool(const command::args_&, const game::netadr_t&, game::msg_t&)>;
	
	void on_command(const std::string & command, const callback & callback);
	void initialize();

	extern bool log_packets;
}