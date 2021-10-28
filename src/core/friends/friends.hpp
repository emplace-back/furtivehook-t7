#pragma once
#include "dependencies/std_include.hpp"

namespace friends
{
	void write_to_friends();
	void refresh_friends();
	void draw_friends_list(const float width, const float spacing);
	void initialize(); 
	
	struct friends_t
	{
		std::uint64_t steam_id;
		std::string name;
		std::string ip_data;
		std::int64_t last_online;
		events::instant_message::response_t response;
	}; 

	extern std::vector<friends_t> friends;
}