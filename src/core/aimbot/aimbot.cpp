#include "dependencies/std_include.hpp"
#include "aimbot.hpp"

namespace aimbot
{
	bool enabled = false, silent = true, auto_fire = true, priority_bonescan = true, asynchronous = true, legit = true;
	Vec3 aim_angles, view_origin;
	bone_target_t* bone_target = nullptr;
	aim_target_t* aim_target = nullptr;
	target_t target[18];
	std::vector<std::uint8_t> ignore_target(18, 0);
	std::vector<std::uint8_t> priority_target(18, 0);
	std::vector<aim_target_t> aim_targets;
	std::vector<bone_target_t> bone_targets;
	std::vector<std::future<float>> penetration_damage;
	std::vector<game::bone_tag> aim_bones
	{
		game::bone_tag::head,
		game::bone_tag::neck,
		game::bone_tag::shoulder_left,
		game::bone_tag::shoulder_right,
		game::bone_tag::spineupper,
		game::bone_tag::spine,
		game::bone_tag::spinelower,
		game::bone_tag::knee_left,
		game::bone_tag::knee_right,
		game::bone_tag::mainroot,
	};

	bool get_best_bone(const game::centity_t* cent, const Vec3& start, const std::vector<Vec3> points, Vec3* pos, float* damage)
	{
		penetration_damage.clear();
		bone_targets.clear();
		bone_target = nullptr;

		if (asynchronous)
		{
			for (const auto& point : points)
			{
				penetration_damage.emplace_back(std::async(&autowall::get_penetration_damage, start, point, &game::cg()->predictedPlayerState, cent->nextState.number));
			}

			for (size_t i = 0; i < points.size(); ++i)
			{
				if (const auto damage = penetration_damage[i].get(); damage > 0.0f)
				{
					bone_targets.emplace_back(damage, points[i]);
				}
			}
		}
		else
		{
			for (const auto& point : points)
			{
				if (const auto damage = autowall::get_penetration_damage(start, point, &game::cg()->predictedPlayerState, cent->nextState.number); damage > 0.0f)
				{
					bone_targets.emplace_back(damage, point);
				}
			}
		}

		if (!bone_targets.empty())
		{
			bone_target = &*std::max_element(bone_targets.begin(), bone_targets.end());

			*damage = bone_target->damage;
			*pos = bone_target->location;

			return true;
		}

		return false;
	}
	
	Vec3 get_best_target_position(const aim_target_t* aim_target)
	{
		const auto priority_target = std::find_if(aim_targets.begin(), aim_targets.end(), [&](const auto& target) { return target.priority; });

		if (priority_target != aim_targets.end())
		{
			return target[priority_target->cent->nextState.number].aim_pos;
		}
		else
		{
			return target[aim_target->cent->nextState.number].aim_pos;
		}
	}
	
	bool is_valid_target(game::centity_t* cent)
	{
		if (!cent->is_alive())
			return false;

		if (cent->nextState.number == game::cg()->predictedPlayerState.clientNum || !game::is_enemy(cent->nextState.number))
			return false;

		return true;
	}
	
	void run(const game::playerState_s* ps)
	{
		aim_targets.clear();
		aim_target = nullptr;

		if (!enabled)
			return;

		game::CG_GetPlayerViewOrigin(ps, &view_origin);

		for (size_t i = 0; i < 18; ++i)
		{
			if (const auto cent = game::centity(i); cent && is_valid_target(cent))
			{
				target[i].point_scan.clear(); 
				
				const auto priority_client = priority_target[i]; 
				
				for (const auto& aim_bone : aim_bones)
					game::AimTarget_GetTagPos(cent, game::get_tag(aim_bone), &target[i].aim_point[static_cast<size_t>(aim_bone)]);

				if (priority_bonescan && priority_client || !priority_bonescan)
				{
					for (const auto& aim_bone : aim_bones)
						target[i].point_scan.emplace_back(target[i].aim_point[static_cast<size_t>(aim_bone)]);
				}
				else
				{
					target[i].point_scan.emplace_back(target[i].aim_point[static_cast<std::uint32_t>(game::bone_tag::head)]);
				}
				
				target[i].vulnerable = get_best_bone(cent, view_origin, target[i].point_scan, &target[i].aim_pos, &target[i].damage);

				if (target[i].vulnerable && !ignore_target[i])
				{
					aim_targets.emplace_back(priority_client, cent, target[i].damage, std::floorf(cent->pose.origin.distance(ps->origin)));
				}
			}
		}

		if (!aim_targets.empty())
		{
			aim_target = &*std::max_element(aim_targets.begin(), aim_targets.end());
			game::vectoangles(&(get_best_target_position(aim_target) - view_origin), &aim_angles);

			aim_angles[0] = math::angle_delta(aim_angles[0], game::cg()->refdefViewAngles[0]);
			aim_angles[1] = math::angle_delta(aim_angles[1], game::cg()->refdefViewAngles[1]);
			aim_angles[2] = math::angle_delta(aim_angles[2], game::cg()->refdefViewAngles[2]);

			if (!silent)
				game::cl()->viewangles += aim_angles;
		}
	}
}