#pragma once
#include "dependencies/std_include.hpp"

namespace steam
{
	bool send_lobby_chat_message(const uint64_t lobby_id, const int type, const std::string& data, const bool add_null_terminator = true);
	std::string get_friend_persona_name(const std::uint64_t steam_id, const std::string & name = "");
	const char* __fastcall get_persona_name(ISteamFriends* thisptr);
	int __fastcall get_lobby_chat_entry(ISteamMatchmaking* thisptr, uint64_t lobby_id, int id, uint64_t* sender_id, void* data, int length, EChatEntryType* type);
	bool read_p2p_packet(ISteamNetworking* thisptr, void* dest, uint32_t dest_size, uint32* msg_size, uint64_t* steam_id, int channel);
	void initialize();
	
	extern std::string persona_name;
}