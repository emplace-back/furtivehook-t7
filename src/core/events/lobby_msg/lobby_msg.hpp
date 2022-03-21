#pragma once
#include "dependencies/std_include.hpp"

namespace events::lobby_msg
{
	bool __fastcall callback_handle_packet(const game::netadr_t & from, game::msg_t & msg, game::LobbyModule module);
	std::string build_lobby_msg(const game::LobbyModule module);
	bool send_to_host_unreliably(const game::msg_t& msg, const game::LobbyModule module);
	bool send_to_client_unreliably(const game::netadr_t & netadr, const std::uint64_t xuid, const game::msg_t & msg, const game::LobbyModule module);
	void initialize();

	extern bool log_messages;
}

namespace std
{
	template<typename T1, typename T2>
	struct hash<std::pair<T1, T2>>
	{
		size_t operator()(const std::pair<T1, T2>& pair) const noexcept
		{
			return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
		}
	};
}