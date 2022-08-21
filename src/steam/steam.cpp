#include "dependencies/std_include.hpp"
#include "steam.hpp"

namespace steam
{
	auto get_persona_name_original = reinterpret_cast<decltype(&get_persona_name)>(0);
	auto get_lobby_chat_entry_original = reinterpret_cast<decltype(&get_lobby_chat_entry)>(0); 
	auto read_p2p_packet_original = reinterpret_cast<decltype(&read_p2p_packet)>(0);
	auto read_p2p_packet_server_original = reinterpret_cast<decltype(&read_p2p_packet)>(0);
	ISteamFriends* friends = nullptr; ISteamMatchmaking* matchmaking = nullptr;
	std::string persona_name = "";

	bool send_lobby_chat_message(const uint64_t lobby_id, const int type, const std::string& data, const bool add_null_terminator)
	{
		auto message{ std::to_string(type) + " " + data };

		if (add_null_terminator)
		{
			message.push_back(0);
		}

		return matchmaking->SendLobbyChatMsg(lobby_id, message.data(), message.size());
	}
	
	std::string get_friend_persona_name(const std::uint64_t steam_id, const std::string& name)
	{
		const auto steam_name = friends->GetFriendPersonaName(steam_id);

		if (steam_name != nullptr
			&& *steam_name
			&& steam_name != "[unknown]"s
			&& steam_name != "[anonymous]"s
			&& steam_name != name)
		{
			return steam_name;
		}

		return "";
	}

	const char* __fastcall get_persona_name(ISteamFriends* thisptr)
	{
		if (!persona_name.empty())
		{
			game::I_strncpyz(
				reinterpret_cast<char*>(game::base_address + 0x114248E0), 
				persona_name.data(), 
				128);
			
			return persona_name.data();
		}

		return get_persona_name_original(thisptr);
	}

	int __fastcall get_lobby_chat_entry(ISteamMatchmaking* thisptr, uint64_t lobby_id, int id, uint64_t* sender_id, void* data, int length, EChatEntryType* type)
	{
		const auto count = get_lobby_chat_entry_original(thisptr, lobby_id, id, sender_id, data, length, type);

		if (count > 0 && *type == k_EChatEntryTypeChatMsg)
		{
			auto message = std::string{ static_cast<char*>(data), std::min(2048ull, static_cast<std::string::size_type>(length)) };
			message = utils::get_chat_message(message.substr(2), *sender_id);

			PRINT_LOG("%s", message.data());

			utils::string::clean_invalid_model_path(static_cast<char*>(data));
		}

		return count;
	}

	bool read_p2p_packet(ISteamNetworking* thisptr, void* dest, uint32_t dest_size, uint32* msg_size, uint64_t* steam_id, int channel)
	{
		const auto result = read_p2p_packet_original(thisptr, dest, dest_size, msg_size, steam_id, channel);

		PRINT_LOG("Received P2P packet from (%llu) of size [%u]", *steam_id, *msg_size);

		if (result)
		{
			return false;
		}

		return result;
	}

	bool read_p2p_packet_server(ISteamNetworking* thisptr, void* dest, uint32_t dest_size, uint32* msg_size, uint64_t* steam_id, int channel)
	{
		const auto result = read_p2p_packet_server_original(thisptr, dest, dest_size, msg_size, steam_id, channel);

		PRINT_LOG("Received P2P packet from (%llu) of size [%u]", *steam_id, *msg_size);

		if (result)
		{
			return false;
		}

		return result;
	}

	void initialize()
	{
		if (friends = *reinterpret_cast<ISteamFriends**>(game::base_address + 0x10BBDBA0))
		{
			get_persona_name_original = utils::hook::vtable(friends, 0, &get_persona_name);
		}

		if (matchmaking = *reinterpret_cast<ISteamMatchmaking**>(game::base_address + 0x10BBDBB0))
		{
			get_lobby_chat_entry_original = utils::hook::vtable(matchmaking, 27, &get_lobby_chat_entry);
		}

		/*if (const auto networking = *reinterpret_cast<ISteamNetworking**>(game::base_address + 0x10BBDBD0))
		{
			read_p2p_packet_original = utils::hook::vtable(networking, 2, read_p2p_packet);
		}

		if (const auto server_networking = *reinterpret_cast<ISteamNetworking**>(game::base_address + 0x1142F168))
		{
			read_p2p_packet_server_original = utils::hook::vtable(server_networking, 2, read_p2p_packet_server);
		}*/
	}
}
