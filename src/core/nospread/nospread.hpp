#pragma once
#include "dependencies/std_include.hpp"

namespace nospread
{
	extern bool enabled;
	Vec3 get_spread_angles(const game::playerState_s* ps, const game::usercmd_s* cmd);
}