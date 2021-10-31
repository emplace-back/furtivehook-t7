#pragma once
#include "dependencies/std_include.hpp"

namespace autowall
{
	float get_penetration_damage(const Vec3& start, const Vec3& target, game::playerState_s* ps, const size_t trace_entity_num);

	extern bool enabled;
}