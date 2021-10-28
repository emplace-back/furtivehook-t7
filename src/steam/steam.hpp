#pragma once
#include "dependencies/std_include.hpp"

namespace steam
{
	std::string get_friend_persona_name(const std::uint64_t steam_id, const std::string & name);
	void initialize();
	
	extern std::string persona_name;
}