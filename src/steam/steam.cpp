#include "dependencies/std_include.hpp"
#include "steam.hpp"

namespace steam
{
	auto get_persona_name_original = reinterpret_cast<decltype(&get_persona_name)>(0);
	auto get_lobby_chat_entry_original = reinterpret_cast<decltype(&get_lobby_chat_entry)>(0); 
	utils::hook::detour live_steam_filter_persona_name_hook;
	utils::hook::detour utf8safe_strncpyz_hook;
	ISteamFriends* friends = nullptr; ISteamMatchmaking* matchmaking = nullptr; ISteamApps* apps = nullptr;
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
	
	void __fastcall live_steam_filter_persona_name(char*, int, bool)
	{
		live_steam_filter_persona_name_hook.clear();
	}

	void __fastcall utf8safe_strncpyz(const char *src, char *dest, int)
	{
		utf8safe_strncpyz_hook.invoke<void>(src, dest, 128);
		utf8safe_strncpyz_hook.clear();
	}

	const char* __fastcall get_persona_name(ISteamFriends* thisptr)
	{
		if (!persona_name.empty())
		{
			live_steam_filter_persona_name_hook.create(game::base_address + 0x1EAF350, live_steam_filter_persona_name);
			utf8safe_strncpyz_hook.create(game::base_address + 0x1EB06C0, utf8safe_strncpyz);
			
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
		}

		return count;
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

		if (apps = *reinterpret_cast<ISteamApps**>(game::base_address + 0x10BBDBC0))
		{
			//utils::hook::return_value((*reinterpret_cast<void***>(game::SteamApps))[6], true); // BIsSubscribedApp
			//utils::hook::return_value((*reinterpret_cast<void***>(game::SteamApps))[7], true); // BIsAppInstalled
		}
	}
}
