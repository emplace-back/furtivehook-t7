#pragma once
#include "dependencies/std_include.hpp"

namespace friends
{	
	struct response_t
	{
		bool valid;
		std::chrono::time_point<std::chrono::system_clock> last_online{};
		game::Msg_InfoResponse info_response;
		game::PresenceData presence_data;
	}; 
	
	struct friend_info
	{
		std::uint64_t steam_id;
		std::string name;
		std::int64_t last_online;
		response_t response;
	}; 

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(friend_info, steam_id, name, last_online);

	using callback = std::function<void(const std::chrono::time_point<std::chrono::system_clock>&, friend_info&)>;
	void for_each_friend(const uint64_t sender_id, const bool ignore, const callback & callback);
	void write_to_friends();
	void draw_friends_list(const float width, const float spacing);
	void initialize();

	extern std::vector<friend_info> friends;
	extern uint32_t NONCE;
}