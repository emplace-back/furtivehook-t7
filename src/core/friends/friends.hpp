#pragma once
#include "dependencies/std_include.hpp"

namespace friends
{	
	struct response_t
	{
		bool valid;
		game::Msg_InfoResponse info_response;
		std::int64_t last_online;
	}; 
	
	struct friend_info
	{
		std::uint64_t steam_id;
		std::string name;
		std::int64_t last_online;
		response_t response;
	}; 

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(friend_info, steam_id, name, last_online);

	void write_to_friends();
	void add_friend_response(const game::Msg_InfoResponse & info_response, const std::uint64_t sender_id);
	void draw_friends_list(const float width, const float spacing);
	void initialize();

	extern std::vector<friend_info> friends;
	extern uint32_t NONCE;
}