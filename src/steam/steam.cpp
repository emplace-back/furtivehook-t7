#include "dependencies/std_include.hpp"
#include "steam.hpp"

namespace steam
{
	utils::hook::detour get_persona_name_hook;
	utils::hook::detour live_steam_filter_persona_name_hook;
	
	ISteamFriends* steam_friends; 
	std::string persona_name = "";

	bool is_steam_id_valid(const std::uint64_t steam_id)
	{
		const CSteamID current_steam_id{ steam_id };
		return current_steam_id.IsValid();
	}
	
	std::string get_friend_persona_name(const std::uint64_t steam_id, const std::string& name)
	{
		if (is_steam_id_valid(steam_id))
		{
			const auto steam_name = steam_friends->GetFriendPersonaName(steam_id);

			if (steam_name != nullptr
				&& *steam_name
				&& steam_name != "[unknown]"s
				&& steam_name != name)
			{
				return steam_name;
			}
		}

		return "";
	}
	
	int __fastcall live_steam_filter_persona_name(char *utf8Name, int len, bool asciionly)
	{
		live_steam_filter_persona_name_hook.clear();
		return 0;
	}

	const char* __fastcall get_persona_name(ISteamFriends* thisptr)
	{
		if (!persona_name.empty())
		{
			live_steam_filter_persona_name_hook.create(game::base_address + 0x1EAF350, live_steam_filter_persona_name);
			return persona_name.data();
		}
		
		return get_persona_name_hook.invoke<const char*>(thisptr);
	}

	void initialize()
	{
		steam_friends = game::LiveSteam_GetFriends(0);
		
		if (steam_friends == nullptr)
		{
			return throw std::runtime_error("Failed to initialize SteamFriends");
		}
		
		get_persona_name_hook.create((*reinterpret_cast<void***>(steam_friends))[0], get_persona_name);
	}
}
