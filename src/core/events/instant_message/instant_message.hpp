#pragma once
#include "dependencies/std_include.hpp"

namespace events::instant_message
{
	bool __fastcall dispatch_message(std::uint64_t sender_id, ControllerIndex_t controller_index, std::uint8_t * message, std::uint32_t message_size); 
	bool send_info_request(const std::vector<std::uint64_t>& recipients);
	void initialize();
	
	extern bool log_messages, prevent_join;
}