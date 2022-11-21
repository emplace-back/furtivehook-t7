#pragma once
#include "dependencies/std_include.hpp"

namespace friends
{	
	struct response_t
	{
		game::Msg_InfoResponse info_response;
		game::PresenceData presence;
	}; 
	
	struct friend_info
	{
		std::uint64_t steam_id;
		std::string name;
		std::time_t last_online;
		response_t response;

		bool is_online() const noexcept
		{
			return response.presence.version || response.info_response.nonce;
		}

		bool operator<(const friend_info& other) const
		{
			return last_online > other.last_online;
		}
	}; 

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(friend_info, steam_id, name, last_online)

	friend_info* get(const uint64_t id);
	void write();
	void draw_friends_list(const float width, const float spacing);
	void initialize();

	extern bool fetch;
	extern std::vector<friend_info> friends;
	extern uint32_t NONCE;
}