#pragma once
#include "dependencies/std_include.hpp"

namespace events::instant_message
{
	using callback = std::function<bool(game::msg_t&, const std::uint64_t)>; 
	
	void send_info_request(const std::vector<std::uint64_t>& recipients);
	void on_message(const std::uint8_t type, const callback& callback);
	void initialize();
	
	extern bool log_messages;
}