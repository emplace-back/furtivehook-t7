#pragma once
#include "dependencies/std_include.hpp"

namespace steam
{
	bool get_p2p_session_state(const uintptr_t* networking, const uint64_t steam_id, P2PSessionState_t* session_state);
	std::string get_sender_string(const std::uint64_t sender_id);
	bool send_crash(const uint64_t xuid);
	bool send_instant_message(const uint64_t xuid, const std::uint8_t type, const std::string& data);
	bool send_instant_message(const uint64_t xuid, const std::uint8_t type, const game::msg_t& msg);
	bool send_lobby_chat_message(const uint64_t lobby_id, const int type, const std::string& data, const bool add_null_terminator = true);
	std::string get_friend_persona_name(const std::uint64_t steam_id, const std::string & name = "");
	int __fastcall get_lobby_chat_entry(ISteamMatchmaking* thisptr, uint64_t lobby_id, int id, uint64_t* sender_id, char* data, int length, EChatEntryType* type);
	bool __fastcall is_dlc_installed(ISteamMatchmaking* thisptr, AppId_t appID);
	bool accept_p2p_session_with_user_stub(ISteamNetworking* networking, uint64_t steam_id);
	void initialize();
	
	extern std::string persona_name;
	extern ISteamNetworking* networking;
	extern std::unordered_map<uint64_t, uint64_t> real_steamid;
}