#pragma once
#include "dependencies/std_include.hpp"

namespace events::instant_message
{
	bool send_info_request(const uint64_t xuid, uint32_t nonce);
	void initialize();
	
	extern bool log_messages;
}