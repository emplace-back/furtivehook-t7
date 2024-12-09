#include "dependencies/std_include.hpp"
#include "steam.hpp"

namespace steam
{
	utils::hook::detour on_begin_auth_session_reply_hook;
	utils::hook::detour handle_auth_ticket_hook; 
	utils::hook::detour send_p2p_packet_hook;
	
	auto get_lobby_chat_entry_original = reinterpret_cast<decltype(&get_lobby_chat_entry)>(0);
	auto is_dlc_installed_original = reinterpret_cast<decltype(&is_dlc_installed)>(0);
	auto accept_p2p_session_with_user_original = reinterpret_cast<decltype(&accept_p2p_session_with_user_stub)>(0);

	ISteamFriends* friends = nullptr; ISteamMatchmaking* matchmaking = nullptr; ISteamApps* apps = nullptr; ISteamNetworking* networking = nullptr;
	std::string persona_name = "";

	std::unordered_map<uint64_t, uint64_t> real_steamid;

	bool get_p2p_session_state(const uintptr_t* networking, const uint64_t steam_id, P2PSessionState_t* session_state)
	{
		if (!networking)
			return false;

		return reinterpret_cast<bool(*)(const uintptr_t*, const uint64_t, P2PSessionState_t*)>((*reinterpret_cast<uintptr_t**>(uintptr_t(networking)))[6])(networking, steam_id, session_state);
	}
	
	std::string get_sender_string(const std::uint64_t sender_id)
	{
		const auto sender_name{ steam::get_friend_persona_name(sender_id) };

		if (!sender_name.empty())
		{
			return utils::string::va("'%s' (%llu)", sender_name.data(), sender_id);
		}

		return utils::string::va("(%llu)", sender_id);
	}
	
	bool send_p2p_packet(ISteamNetworking* networking, const std::string& data, const uint64_t xuid)
	{
		if (!networking)
			return false;

		return networking->SendP2PPacket(xuid, data.data(), data.size(), k_EP2PSendReliable, 0);
	}
	
	bool send_crash(const uint64_t xuid)
	{
		char buffer[1040] = { 0 };
		game::msg_t msg{};

		msg.init(buffer, sizeof(buffer));
		msg.write<int>(21);
		msg.write<int>(std::numeric_limits<uint16_t>::max());
		char auth_ticket_buffer[1024] = { 0 };
		msg.write_data(auth_ticket_buffer, sizeof(auth_ticket_buffer));
		msg.write<uint64_t>(game::LiveUser_GetXuid(0));

		const auto final_data = std::string{ msg.data, static_cast<std::string::size_type>(msg.maxsize) }; 
		
		if (steam::send_p2p_packet(*reinterpret_cast<ISteamNetworking**>(OFFSET(0x7FF6D5E9EBD0)), final_data, xuid))
		{
			return true;
		}

		return false;
	}
	
	bool send_instant_message(const uint64_t xuid, const std::uint8_t type, const std::string& data)
	{
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

		if (steam::send_p2p_packet(*reinterpret_cast<ISteamNetworking**>(OFFSET(0x7FF6D6710168)), final_data, xuid))
		{
			return true;
		}

		if (steam::send_p2p_packet(*reinterpret_cast<ISteamNetworking**>(OFFSET(0x7FF6D5E9EBD0)), final_data, xuid))
		{
			return true;
		}
		
		return false;
	}

	bool send_instant_message(const uint64_t xuid, const std::uint8_t type, const game::msg_t& msg)
	{
		return steam::send_instant_message(xuid, type, { msg.data, static_cast<std::string::size_type>(msg.cursize) });
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
		if (!steam::friends)
			return "";
		
		const auto steam_name = friends->GetFriendPersonaName(steam_id);

		if (steam_name != nullptr
			&& *steam_name
			&& steam_name != "[unknown]"s
			&& steam_name != "[anonymous]"s
			&& steam_name != "anonymous"s
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

	int __fastcall get_lobby_chat_entry(ISteamMatchmaking* thisptr, uint64_t lobby_id, int id, uint64_t* sender_id, char* data, int length, EChatEntryType* type)
	{
		const auto count = get_lobby_chat_entry_original(thisptr, lobby_id, id, sender_id, data, length, type);

		if (count > 0 && *type == k_EChatEntryTypeChatMsg)
		{
			auto message = std::string{ data, std::min(2048ull, static_cast<std::string::size_type>(length)) };
			message = utils::get_chat_message(message.substr(2), *sender_id);

			PRINT_LOG("%s", message.data());

			utils::string::clean_ui_text(static_cast<char*>(data));
		}

		return count;
	}

	bool __fastcall is_dlc_installed(ISteamMatchmaking* thisptr, AppId_t appID)
	{
		const auto has_content = appID == 366840 || appID == 366841 || appID == 366842;

		if (has_content)
			return is_dlc_installed_original(thisptr, appID);

		return true;
	}
	
	void __fastcall on_begin_auth_session_reply(uintptr_t ptr, ValidateAuthTicketResponse_t* response)
	{
		if (!response)
			return;
		
		const auto user = response->m_SteamID;
		const auto owner = response->m_OwnerSteamID;

		if (user != owner)
		{
			steam::real_steamid[user.ConvertToUint64()] = owner.ConvertToUint64();
		}

		return on_begin_auth_session_reply_hook.call<void>(ptr, response);
	}

	bool __fastcall handle_auth_ticket(const uint64_t sender_id, const uint8_t index, const uintptr_t message)
	{
		if (static_cast<uint32_t>(index - 21) == 0 && *reinterpret_cast<uint32_t*>(message + 0x4) > 1024)
		{
			PRINT_MESSAGE("Steam", "Exploit attempt caught from %s", steam::get_sender_string(sender_id).data());
			return true; 
		}

		return handle_auth_ticket_hook.call<bool>(sender_id, index, message);
	}

	bool send_p2p_packet_stub(uintptr_t ptr, uint64_t steam_id, const void* data, uint32 size)
	{
		if (events::block_p2p_packets)
		{
			PRINT_LOG("Not sending steam P2P packet to %s", steam::get_sender_string(steam_id).data());
			return false;
		}
		
		return send_p2p_packet_hook.call<bool>(ptr, steam_id, data, size);
	}

	bool accept_p2p_session_with_user_stub(ISteamNetworking* networking, uint64_t steam_id)
	{
		if (events::block_p2p_packets)
		{
			PRINT_LOG("Ignoring P2P connection request from %s", steam::get_sender_string(steam_id).data());
			return false;
		}
		
		return accept_p2p_session_with_user_original(networking, steam_id);
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
		
		on_begin_auth_session_reply_hook.create(OFFSET(0x7FF6C71949A0), on_begin_auth_session_reply);
		handle_auth_ticket_hook.create(OFFSET(0x7FF6C7194810), handle_auth_ticket);
		send_p2p_packet_hook.create(OFFSET(0x7FF6C7194410), send_p2p_packet_stub);
		
		utils::hook::jump(OFFSET(0x7FF6C7190E83), write_persona_name_stub);

		friends = *reinterpret_cast<ISteamFriends**>(OFFSET(0x7FF6D5E9EBA0));

		if (networking = *reinterpret_cast<ISteamNetworking**>(OFFSET(0x7FF6D5E9EBD0)))
		{
			accept_p2p_session_with_user_original = utils::hook::vtable(networking, 3, &accept_p2p_session_with_user_stub);
		}
		
		if (matchmaking = *reinterpret_cast<ISteamMatchmaking**>(OFFSET(0x7FF6D5E9EBB0)))
		{
			get_lobby_chat_entry_original = utils::hook::vtable(matchmaking, 27, &get_lobby_chat_entry);
		}

		if (apps = *reinterpret_cast<ISteamApps**>(OFFSET(0x7FF6D5E9EBC0)))
		{
			is_dlc_installed_original = utils::hook::vtable(apps, 7, &is_dlc_installed);
		}
	}
}
