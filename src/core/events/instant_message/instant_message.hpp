#pragma once
#include "dependencies/std_include.hpp"

namespace events::instant_message
{
	bool __fastcall dw_instant_dispatch_message_stub(std::uint64_t sender_id, ControllerIndex_t controller_index, std::uint8_t * message, std::uint32_t message_size);
	bool send_info_request(const std::uint64_t id, uint32_t nonce);
	void initialize();
	
	extern bool log_messages;
}