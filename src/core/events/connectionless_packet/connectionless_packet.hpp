#pragma once
#include "dependencies/std_include.hpp"

namespace events::connectionless_packet
{
	using callback = std::function<bool(const command::args&, const game::netadr_t&, game::msg_t& msg)>;
	
	void on_command(const std::string& command, const callback& callback);
	bool __fastcall handle_command(const game::netadr_t & from, game::msg_t * msg, const bool sv = false);
	void initialize();
	extern bool log_commands;
}