#include "dependencies/std_include.hpp"
#include "autowall.hpp"

namespace autowall
{
	bool enabled = true;
	
	bool can_hit_entity(const game::Weapon& weapon)
	{
		if (!weapon.weaponData)
			return false;
		
		return game::BG_GetWeaponDef(weapon)->weapType == game::WEAPTYPE_BULLET;
	}

	bool trace_hit(const game::trace_t& trace)
	{
		if (const auto victim = game::Trace_GetEntityHitId(&trace); victim < 18)
		{
			return aimbot::ignore_target[victim];
		}

		return false;
	}
	
	bool hit_bad_entity(const game::BulletTraceResults& br)
	{
		return trace_hit(br.trace);
	}
	
	float get_weapon_hit_location_multiplier(const game::Weapon& weapon, const uint16_t hit_location)
	{
		if (hit_location >= game::HITLOC_NUM)
			return 1.0f;
		
		if (!weapon.weaponData)
			return game::g_fHitLocDamageMult[hit_location];

		return game::BG_GetWeaponHitLocationMultiplier(weapon, hit_location);
	}
	
	float get_bullet_damage(const game::Weapon& weapon, const game::BulletTraceResults& br, const game::BulletFireParams& bt)
	{
		const auto multiplier = game::G_GetWeaponHitLocationMultiplier(br.trace.partGroup, weapon);
		const auto range_damage = static_cast<float>(game::BG_GetWeaponDamageForRange(weapon, &bt.origStart, &br.hitPos));
		return std::fmax(multiplier * range_damage, 1.0f) * bt.damageMultiplier;
	}

	float get_bullet_depth(const game::BulletTraceResults& br, const game::BulletTraceResults& rev_br, const game::BulletFireParams& rev_bp, const Vec3& hit_pos, const bool all_solid)
	{
		game::BulletHitInfo info{};
		info.allSolid = all_solid;
		info.br.hitPos = hit_pos;
		
		return game::BG_GetDepth(&info, &br, &rev_bp, &rev_br);
	}
	
	float get_penetration_damage(const Vec3& start, const Vec3& target, game::playerState_s* ps, const size_t trace_entity_num)
	{
		const auto weapon = game::BG_GetViewmodelWeaponIndex(ps);
		
		if (!can_hit_entity(weapon))
			return 0.0f; 
		
		if (target.distance(start) >= game::get_weapon_damage_range(weapon))
			return 0.0f; 
		
		game::BulletFireParams bp;
		bp.weaponEntIndex = 1022;
		bp.ignoreEntIndex = ps->clientNum;
		bp.damageMultiplier = 1.0f;
		bp.origStart = start;
		bp.start = start;
		bp.end = target;
		bp.dir = (target - start).normalized();

		game::BulletTraceResults br{};
		
		if (!game::CG_BulletTrace(&br, &bp, ps->clientNum, 0))
			return 0.0f;

		if (hit_bad_entity(br))
			return 0.0f;

		if (trace_entity_num == game::Trace_GetEntityHitId(&br.trace))
			return get_bullet_damage(weapon, br, bp);

		const auto penetrate_type = game::BG_GetPenetrateType(weapon);

		if (penetrate_type == game::PENETRATE_TYPE_NONE || game::BG_GetWeaponDef(weapon)->bBulletImpactExplode)
			return 0.0f;
		
		constexpr auto perk_bulletPenetrationMultiplier = 2.0f;
		const auto has_fmj = game::CG_ClientHasPerk(0, ps->clientNum, 9);

		for (size_t i = 0; i < 5; ++i)
		{
			auto max_depth = game::BG_GetSurfacePenetrationDepth(penetrate_type, br.depthSurfaceType);

			if (has_fmj)
				max_depth *= perk_bulletPenetrationMultiplier;

			if (br.trace.sflags & 4)
				max_depth = 100.0f;

			if (max_depth <= 0.0f)
				return 0.0f;

			if (br.trace.sflags & 0x100)
				return 0.0f;

			if (hit_bad_entity(br))
				return 0.0f;

			const auto last_hit_pos = br.hitPos;

			if (!game::BG_AdvanceTrace(&bp, &br, 0.259f))
				return 0.0f;

			const auto traceHitB = game::CG_BulletTrace(&br, &bp, ps->clientNum, br.depthSurfaceType);

			if (hit_bad_entity(br))
				return 0.0f;

			game::BulletFireParams rev_bp;
			rev_bp.weaponEntIndex = bp.weaponEntIndex;
			rev_bp.ignoreEntIndex = bp.ignoreEntIndex;
			rev_bp.damageMultiplier = bp.damageMultiplier;
			rev_bp.origStart = bp.origStart;
			rev_bp.dir = -bp.dir;
			rev_bp.start = bp.end;
			rev_bp.end = rev_bp.dir * 0.01f + last_hit_pos;

			auto rev_br = br;
			rev_br.trace.normal = -rev_br.trace.normal;

			if (traceHitB)
				game::BG_AdvanceTrace(&rev_bp, &rev_br, 0.01f);

			const auto rev_trace_hit = game::CG_BulletTrace(&rev_br, &rev_bp, ps->clientNum, rev_br.depthSurfaceType);
			const auto all_solid = rev_trace_hit && rev_br.trace.allSolid || br.trace.startSolid && rev_br.trace.startSolid;
			auto depth = 0.0f;
			
			if (rev_trace_hit || all_solid)
			{
				if (rev_trace_hit)
				{
					auto new_depth = game::BG_GetSurfacePenetrationDepth(penetrate_type, rev_br.depthSurfaceType);

					if (has_fmj)
						new_depth *= perk_bulletPenetrationMultiplier;

					max_depth = std::fmin(max_depth, new_depth);

					if (max_depth <= 0.0f)
						return 0.0f;

					if (rev_br.trace.sflags & 0x100)
						return 0.0f;
				}

				depth = get_bullet_depth(br, rev_br, rev_bp, last_hit_pos, all_solid);
				
				bp.damageMultiplier -= depth / max_depth;

				if (bp.damageMultiplier <= 0.0f)
					return 0.0f;

				if (!all_solid)
				{
					if (!hit_bad_entity(br) && game::Trace_GetEntityHitId(&br.trace) == trace_entity_num)
						return get_bullet_damage(weapon, br, bp);
				}
			}

			if (!traceHitB)
			{
				if (hit_bad_entity(br))
					return 0.0f; 
				
				return get_bullet_damage(weapon, br, bp);
			}

			if (!rev_trace_hit && !all_solid)
			{
				const static auto bg_bulletPenetrationTreatVoidsAsSolid = *reinterpret_cast<game::dvar_t**>(game::base_address + 0x19C2BA30);
				
				if (bg_bulletPenetrationTreatVoidsAsSolid->current.integer & 8)
				{
					depth = get_bullet_depth(br, br, bp, last_hit_pos, all_solid);
				}
			}
		}

		if (!hit_bad_entity(br) && game::Trace_GetEntityHitId(&br.trace) == trace_entity_num)
			return get_bullet_damage(weapon, br, bp);

		return 0.0f;
	}
}