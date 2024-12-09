#pragma once

namespace events::lobby_msg
{
	using callback = std::function<bool(const game::netadr_t&, game::msg_t&, game::LobbyModule module)>; 
	
	void on_message(const game::LobbyModule module, const game::MsgType type, const callback& callback);
	std::string build_lobby_msg(const game::LobbyModule module);
	bool send_lobby_msg(const game::NetChanMsgType channel, const game::LobbyModule module, const game::msg_t& msg, const game::netadr_t& netadr, const uint64_t src_xuid = 0);
	bool send_lobby_msg(const game::LobbyModule module, const game::msg_t& msg, const game::netadr_t& netadr, const uint64_t src_xuid = 0);
	void initialize();

	extern bool log_messages;

	extern constexpr auto PROTOCOL{ 0x4864 };
}

namespace std
{
	template<typename T1, typename T2>
	struct hash<std::pair<T1, T2>>
	{
		size_t operator()(const std::pair<T1, T2>& pair) const
		{
			return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
		}
	};
}