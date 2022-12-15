#include "dependencies/std_include.hpp"
#include "steam.hpp"

namespace steam
{
	auto get_lobby_chat_entry_original = reinterpret_cast<decltype(&get_lobby_chat_entry)>(0);
	ISteamFriends* friends = nullptr; ISteamMatchmaking* matchmaking = nullptr;
	bool block_p2p_packets = false;
	std::string persona_name = "";
	
	bool send_p2p_packet(uint64_t xuid, const std::uint8_t type, const std::string& data)
	{
		const auto send_packet = [=](ISteamNetworking* networking, const std::string& data, const uint64_t xuid)
		{
			if (!networking)
				return false;

			return networking->SendP2PPacket(xuid, data.data(), data.size(), k_EP2PSendReliable, 0);
		};

		if (data.size() > 1024)
			return false;

		char buffer[1032] = { 0 };
		game::msg_t msg{};

		msg.init(buffer, sizeof(buffer));
		msg.write<int>('(');
		msg.write<uint8_t>('1');
		msg.write<uint8_t>(type);
		msg.write_data(data.data(), data.size());
		*reinterpret_cast<int*>(&msg.data[1028]) = msg.cursize;

		const auto final_data = std::string{ msg.data, static_cast<std::string::size_type>(msg.maxsize) };

		if (send_packet(*reinterpret_cast<ISteamNetworking**>(OFFSET(0x7FF6D6710168)), final_data, xuid))
		{
			return true;
		}

		if (send_packet(*reinterpret_cast<ISteamNetworking**>(OFFSET(0x7FF6D5E9EBD0)), final_data, xuid))
		{
			return true;
		}
		
		return false;
	}

	bool send_p2p_packet(uint64_t xuid, const std::uint8_t type, const game::msg_t& msg)
	{
		return steam::send_p2p_packet(xuid, type, { msg.data, static_cast<std::string::size_type>(msg.cursize) });
	}
	
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
		game::I_strncpyz(reinterpret_cast<char*>(OFFSET(0x7FF6D67058E0)), name, 128);
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

	bool __fastcall is_p2p_packet_available(ISteamNetworking* thisptr, uint32* pcubMsgSize)
	{
		const auto available = thisptr->IsP2PPacketAvailable(pcubMsgSize, 0);
		
		if (steam::block_p2p_packets && available)
		{
			return false;
		}
		
		return available;
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
			a.jmp(OFFSET(0x7FF6C7190EA5));
		}); 
		
		utils::hook::jump(OFFSET(0x7FF6C7190E83), write_persona_name_stub);
		
		utils::hook::call(OFFSET(0x7FF6C719417B), is_p2p_packet_available);
		utils::hook::nop(OFFSET(0x7FF6C719417B) + 5, 1);
		
		utils::hook::call(OFFSET(0x7FF6C719707A), is_p2p_packet_available);
		utils::hook::nop(OFFSET(0x7FF6C719707A) + 5, 1);

		friends = *reinterpret_cast<ISteamFriends**>(OFFSET(0x7FF6D5E9EBA0));
		
		if (matchmaking = *reinterpret_cast<ISteamMatchmaking**>(OFFSET(0x7FF6D5E9EBB0)))
		{
			get_lobby_chat_entry_original = utils::hook::vtable(matchmaking, 27, &get_lobby_chat_entry);
		}
	}
}
