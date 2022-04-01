#include "dependencies/std_include.hpp"
#include "steam.hpp"

namespace steam
{
	utils::hook::detour get_persona_name_hook;
	utils::hook::detour is_app_installed_hook;
	utils::hook::detour live_steam_filter_persona_name_hook;
	utils::hook::detour utf8safe_strncpyz_hook;
	
	ISteamFriends* steam_friends; 
	ISteamApps* steam_apps;
	std::string persona_name = "";
	
	std::string get_friend_persona_name(const std::uint64_t steam_id, const std::string& name)
	{
		const auto steam_name = steam_friends->GetFriendPersonaName(steam_id);

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
	
	void __fastcall live_steam_filter_persona_name(char *, int size, bool)
	{
		live_steam_filter_persona_name_hook.clear();
	}

	void __fastcall utf8safe_strncpyz(const char *src, char *dest, int size)
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
		
		return get_persona_name_hook.invoke<const char*>(thisptr);
	}

	void initialize()
	{
		steam_friends = game::LiveSteam_GetFriends(0);
		steam_apps = game::LiveSteam_GetApps(0);
		
		if (steam_friends == nullptr || steam_apps == nullptr)
		{
			throw std::runtime_error{ "Failed to initialize Steam" };
		}
		
		get_persona_name_hook.create((*reinterpret_cast<void***>(steam_friends))[0], get_persona_name);

		//utils::hook::return_value((*reinterpret_cast<void***>(steam_apps))[6], true); // BIsSubscribedApp
		//utils::hook::return_value((*reinterpret_cast<void***>(steam_apps))[7], true); // BIsAppInstalled
	}
}
