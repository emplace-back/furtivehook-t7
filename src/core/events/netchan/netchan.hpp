#pragma once
#include "dependencies/std_include.hpp"

namespace events::netchan
{
	using callback = std::function<bool(const game::netadr_t&, game::LobbyMsg&, const std::uint64_t)>;

	void on_message(const game::MsgType& msg_type, const callback & callback);
	void initialize();

	extern bool log_messages;
}