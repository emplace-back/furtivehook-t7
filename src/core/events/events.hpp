#pragma once
#include "dependencies/std_include.hpp"

namespace events
{
	void initialize();
	extern bool prevent_join, no_presence, spoof_ip, block_p2p_packets;
	extern std::unordered_map<game::netadr_t, std::string> real_addr;
}