#include "dependencies/std_include.hpp"
#include "nospread.hpp"

namespace nospread
{
	bool enabled = true;
	
	Vec3 get_bullet_end_pos(int seed, const Vec3 view_axis[3], const float aim_spread_amount, const float range, const game::Weapon& weapon)
	{
		Vec3 start{}, end{}, direction{};
		game::CG_SimulateBulletFire_EndPos(&seed, 0.0f, aim_spread_amount, &start, &end, &direction, 0.0f, 360.0f, &view_axis[0], &view_axis[1], &view_axis[2], range, weapon, 0, 1);
		game::vectoangles(&direction, &direction);

		return direction;
	}
	
	float get_aim_spread_amount(const game::playerState_s* ps, const game::Weapon& weapon)
	{
		float min_spread, max_spread;
		game::BG_GetSpreadForWeapon(ps, weapon, &min_spread, &max_spread);

		if (ps->fWeaponPosFrac == 1.0f)
		{
			min_spread = game::BG_GetWeaponDef(weapon)->fAdsSpread;
		}
		
		const auto aim_spread_scale = game::cg()->aimSpreadScale / 255.0f;
		return (max_spread - min_spread) * aim_spread_scale + min_spread;
	}
	
	Vec3 get_spread_angles(const game::playerState_s* ps, const game::usercmd_s* cmd)
	{
		auto rand_seed{ cmd->serverTime }; 
		
		Vec3 view_axis[3];
		game::AngleVectors(&game::cg()->refdefViewAngles, &view_axis[0], &view_axis[1], &view_axis[2]);

		auto& spread_angles = get_bullet_end_pos(
			game::BG_seedRandWithGameTime(&rand_seed),
			view_axis, 
			get_aim_spread_amount(ps, cmd->weapon),
			game::get_weapon_damage_range(cmd->weapon),
			cmd->weapon);
		
		spread_angles = math::angle_delta(game::cg()->baseGunAngles, spread_angles);

		return spread_angles;
	}
}