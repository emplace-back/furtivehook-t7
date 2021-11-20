#include "dependencies/std_include.hpp"
#include "nospread.hpp"

namespace nospread
{
	bool enabled = true;
	
	void CG_SimulateBulletFire_EndPos(unsigned int* randSeed, float maxSpread, Vec3* dir, const Vec3 viewAxis[3], float maxRange, const game::Weapon& weapon)
	{
		const static auto CG_SimulateBulletFire_EndPos = reinterpret_cast<void(*)(unsigned int*, float, float, const Vec3*, Vec3*, Vec3*, float, float, const Vec3*, const Vec3*, const Vec3*, float, game::Weapon, int, int)>(game::base_address + 0x123CB50);
		
		Vec3 start, end;
		return CG_SimulateBulletFire_EndPos(randSeed, 0.0f, maxSpread, &start, &end, dir, 0.0f, 360.0f, &viewAxis[0], &viewAxis[1], &viewAxis[2], maxRange, weapon, 0, 1);
	}
	
	float get_aim_spread_amount(const game::Weapon weapon)
	{
		float min_spread, max_spread;
		game::BG_GetSpreadForWeapon(&game::cg()->predictedPlayerState, weapon, &min_spread, &max_spread);

		const auto weapon_pos_frac = game::cg()->predictedPlayerState.weapon_pos_frac();

		if (weapon_pos_frac == 1.0f)
		{
			min_spread = game::BG_GetWeaponDef(weapon)->ads_spread();
		}
		
		const auto aim_spread_scale = game::cg()->aim_spread_scale() / 255.0f;
		return (max_spread - min_spread) * aim_spread_scale + min_spread;
	}
	
	Vec3 get_spread_angles(std::uint32_t rand_seed, const game::Weapon& weapon)
	{
		game::BG_seedRandWithGameTime(&rand_seed);

		Vec3 view_axis[3], angles;
		game::AngleVectors(&game::cg()->refdefViewAngles, &view_axis[0], &view_axis[1], &view_axis[2]);

		const auto weapon_damage_range = game::get_weapon_damage_range(weapon); 
		const auto aim_spread_amount = get_aim_spread_amount(weapon);

		CG_SimulateBulletFire_EndPos(&rand_seed, aim_spread_amount, &angles, view_axis, weapon_damage_range, weapon);
		game::vectoangles(&angles, &angles);
		
		angles[0] = math::angle_delta(game::cg()->refdefViewAngles[0], angles[0]);
		angles[1] = math::angle_delta(game::cg()->refdefViewAngles[1], angles[1]);
		angles[2] = math::angle_delta(game::cg()->refdefViewAngles[2], angles[2]);

		return angles;
	}
}