#pragma once
#include "dependencies/std_include.hpp"

namespace events::lobby_msg
{
	using callback = std::function<bool(const game::netadr_t&, game::LobbyMsg&, const std::uint64_t)>;

	void on_message(const game::LobbyModule& lobby_module, const callback& callback);
	void initialize();

	extern bool log_messages;
}