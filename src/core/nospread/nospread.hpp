#pragma once
#include "dependencies/std_include.hpp"

namespace nospread
{
	extern bool enabled;
	Vec3 get_spread_angles(std::uint32_t rand_seed, const game::Weapon& weapon);
}