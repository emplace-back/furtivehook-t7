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

	void __fastcall write_persona_name(const char* steam_name)
	{
		const auto* name = !persona_name.empty() ? persona_name.data() : steam_name;
		game::I_strncpyz(reinterpret_cast<char*>(game::base_address + 0x114248E0), name, 128);
	}

	int __fastcall get_lobby_chat_entry(ISteamMatchmaking* thisptr, uint64_t lobby_id, int id, uint64_t* sender_id, void* data, int length, EChatEntryType* type)
	{
		const auto count = get_lobby_chat_entry_original(thisptr, lobby_id, id, sender_id, data, length, type);

		if (count > 0 && *type == k_EChatEntryTypeChatMsg)
		{
			auto message = std::string{ static_cast<char*>(data), std::min(2048ull, static_cast<std::string::size_type>(length)) };
			message = utils::get_chat_message(message.substr(2), *sender_id);

			PRINT_LOG("%s", message.data());
		}

		return count;
	}

	bool __fastcall read_p2p_packet(ISteamNetworking* thisptr, void* dest, uint32_t dest_size, uint32* msg_size, uint64_t* steam_id, int channel)
	{
		const auto result = read_p2p_packet_original(thisptr, dest, dest_size, msg_size, steam_id, channel);

		PRINT_LOG("Received P2P packet from (%llu) of size [%u]", *steam_id, *msg_size);

		if (result)
		{
			return false;
		}

		return result;
	}

	bool __fastcall read_p2p_packet_server(ISteamNetworking* thisptr, void* dest, uint32_t dest_size, uint32* msg_size, uint64_t* steam_id, int channel)
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
		const auto write_persona_name_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(rcx);
			a.mov(rcx, rax);
			a.call_aligned(steam::write_persona_name);
			a.pop(rcx);

			a.mov(ebp, 32);
			a.jmp(game::base_address + 0x1EAFEA5);
		}); 
		
		utils::hook::jump(game::base_address + 0x1EAFE7C, write_persona_name_stub);
		utils::hook::nop(game::base_address + 0x1EAFE7C + 5, 2);

		if (matchmaking = *reinterpret_cast<ISteamMatchmaking**>(game::base_address + 0x10BBDBB0))
		{
			get_lobby_chat_entry_original = utils::hook::vtable(matchmaking, 27, &get_lobby_chat_entry);
		}
	}
}
