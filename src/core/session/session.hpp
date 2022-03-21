#pragma once
#include "dependencies/std_include.hpp"

namespace session
{
	void draw_session_list(const float width, const float spacing);
	void register_session(const game::MatchMakingInfo& info);
}