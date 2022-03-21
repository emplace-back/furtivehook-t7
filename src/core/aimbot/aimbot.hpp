#pragma once
#include "dependencies/std_include.hpp"

namespace aimbot
{
	struct target_t
	{
		Vec3 aim_point[static_cast<std::uint32_t>(game::bone_tag::num_tags)], aim_pos;
		std::vector<Vec3> point_scan, points;
		bool vulnerable;
		float damage;
	};

	struct bone_target_t
	{
		float damage;
		Vec3 location;

		bone_target_t() = delete;
		bone_target_t(const float _damage, const Vec3 _location) : damage(_damage), location(_location) {}

		bool operator<(const bone_target_t& other) const
		{
			return other.damage > damage;
		}
	};

	struct aim_target_t
	{
		bool priority;
		const game::centity_t* cent;
		float damage;
		float distance;

		aim_target_t() = delete;
		aim_target_t(const bool _priority, const game::centity_t* _cent, const float _damage, const float _distance) : priority(_priority), cent(_cent), damage(_damage), distance(_distance) {}

		bool operator<(const aim_target_t& other) const
		{
			return other.damage > damage || (other.damage == damage && other.distance < distance) || (other.damage == damage && other.distance == distance);
		}
	};

	extern bool enabled, silent, auto_fire, priority_bonescan, legit;
	extern Vec3 aim_angles, view_origin;
	extern aim_target_t* aim_target;
	extern target_t target[18];
	extern std::vector<std::uint8_t> priority_target;
	extern std::vector<std::uint8_t> ignore_target;
	extern std::vector<game::bone_tag> aim_bones;

	bool is_valid_target(game::centity_t * cent);
	void run(const game::playerState_s * ps);
}