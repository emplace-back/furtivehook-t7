#pragma once
#include "dependencies/std_include.hpp"

namespace events::instant_message
{
	struct response_t
	{
		bool valid;
		std::uint64_t host_xuid;
		std::string host_name;
		game::LobbyMainMode main_mode;
		game::XSESSION_INFO session_info;
		std::int64_t last_online;
	}; 
	
	using callback = std::function<bool(const std::uint64_t, game::msg_t&)>;

	void on_message(const std::uint8_t command, const callback & callback);
	bool handle_message(const std::uint64_t sender_id, const char * message, const std::uint32_t message_size);
	void log_instant_messages(const std::uint64_t sender_id, const std::uint32_t message_size, const std::uint8_t type);
	void send_crash(const std::uint64_t target_steam_id);
	void initialize();

	extern bool log_messages;
}