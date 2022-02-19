#pragma once
#include "dependencies/std_include.hpp"

namespace events::lobby_msg
{
	using callback = std::function<bool(const game::netadr_t&, game::msg_t&)>;

	void on_message(const game::LobbyModule& lobby_module, const callback& callback);
	std::string build_lobby_msg(const game::LobbyModule module);
	bool send_to_host_unreliably(const game::msg_t& msg, const game::LobbyModule module);
	bool send_to_client_unreliably(const game::netadr_t & netadr, const std::uint64_t xuid, const game::msg_t & msg, const game::LobbyModule module);
	void initialize();

	extern bool log_messages;
}