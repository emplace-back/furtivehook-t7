#pragma once
#include "dependencies/std_include.hpp"

namespace events::instant_message
{
	bool send_info_request(const std::vector<std::uint64_t>& recipients, uint32_t nonce);
	void initialize();
	
	extern bool log_messages;
}