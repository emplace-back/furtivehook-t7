#include "dependencies/std_include.hpp"
#include "autowall.hpp"

namespace autowall
{
	bool enabled = true;
	
	bool CG_BulletTrace(game::BulletTraceResults* br, game::BulletFireParams* bp, const int attacker_entity_num, int lastSurfaceType)
	{
		const static auto CG_BulletTrace = reinterpret_cast<bool(__fastcall*)(LocalClientNum_t, game::BulletFireParams *, game::BulletTraceResults*, const game::Weapon, int, int)>(game::base_address + 0x1168BA0);
		return CG_BulletTrace(0, bp, br, {}, attacker_entity_num, lastSurfaceType);
	}
	
	bool treat_as_solid(const game::BulletTraceResults& br, const game::BulletTraceResults& rev_br)
	{
		return br.trace.sflags & 4096 && rev_br.trace.sflags & 4096 && game::bg_bulletPenetrationTreatVoidsAsSolid->current.integer
			|| rev_br.trace.sflags & 4096 && game::bg_bulletPenetrationTreatVoidsAsSolid->current.integer & 2
			|| br.trace.sflags & 4096 && game::bg_bulletPenetrationTreatVoidsAsSolid->current.integer & 4;
	}
	
	float get_bullet_damage(const game::Weapon weapon, const game::BulletTraceResults& br, const game::BulletFireParams& bt)
	{
		const auto multiplier = game::G_GetWeaponHitLocationMultiplier(br.trace.partGroup, weapon);
		const auto range_damage = static_cast<float>(game::BG_GetWeaponDamageForRange(weapon, &bt.origStart, &br.hitPos));
		return std::fmax(multiplier * range_damage, 1.0f) * bt.damageMultiplier;
	}

	float get_bullet_depth(const game::BulletTraceResults& br, const game::BulletTraceResults& rev_br, const game::BulletFireParams& rev_bp, const Vec3& hit_pos, const bool all_solid)
	{
		auto depth{ all_solid ? rev_bp.end.distance(rev_bp.start) : hit_pos.distance(rev_br.hitPos) };

		if (treat_as_solid(br, rev_br))
			depth = hit_pos.distance(br.hitPos);

		return std::fmax(depth, 1.0f);
	}
	
	float get_penetration_damage(const Vec3& start, const Vec3& target, game::playerState_s* ps, const size_t trace_entity_num)
	{
		const auto weapon = game::BG_GetViewmodelWeaponIndex(ps);
		
		game::BulletFireParams bp;
		bp.weaponEntIndex = 1022;
		bp.ignoreEntIndex = ps->clientNum;
		bp.damageMultiplier = 1.0f;
		bp.origStart = start;
		bp.start = start;
		bp.end = target;
		bp.dir = (target - start).normalized();

		game::BulletTraceResults br{};
		
		if (!CG_BulletTrace(&br, &bp, ps->clientNum, 0))
			return 0.0f;

		if (trace_entity_num == game::Trace_GetEntityHitId(&br.trace))
			return get_bullet_damage(weapon, br, bp);

		const auto penetrate_type = game::BG_GetPenetrateType(weapon);

		if (penetrate_type == game::PENETRATE_TYPE_NONE || game::BG_GetWeaponDef(weapon)->bBulletImpactExplode)
			return 0.0f;
		
		constexpr auto perk_bulletPenetrationMultiplier = 2.0f;
		const auto has_fmj = game::CG_ClientHasPerk(0, ps->clientNum, 9);

		for (size_t i = 0; i != game::sv_penetrationCount->current.integer; ++i)
		{
			auto max_depth = game::BG_GetSurfacePenetrationDepth(penetrate_type, br.depthSurfaceType);

			if (has_fmj)
				max_depth *= perk_bulletPenetrationMultiplier;

			if (max_depth <= 0.0f)
				return 0.0f;

			const auto lastHitPos = br.hitPos;

			if (!game::BG_AdvanceTrace(&bp, &br, 0.259f))
				return 0.0f;

			const auto traceHitB = CG_BulletTrace(&br, &bp, ps->clientNum, br.depthSurfaceType);

			game::BulletFireParams revBp;
			revBp.weaponEntIndex = bp.weaponEntIndex;
			revBp.ignoreEntIndex = bp.ignoreEntIndex;
			revBp.damageMultiplier = bp.damageMultiplier;
			revBp.origStart = bp.origStart;
			revBp.dir = -bp.dir;
			revBp.start = bp.end;
			revBp.end = lastHitPos + revBp.dir * 0.01f;

			auto revBr = br;
			revBr.trace.normal = -revBr.trace.normal;

			if (traceHitB)
				game::BG_AdvanceTrace(&revBp, &revBr, 0.01f);

			const auto revTraceHit = CG_BulletTrace(&revBr, &revBp, ps->clientNum, revBr.depthSurfaceType);
			const auto allSolid = revTraceHit && revBr.trace.allSolid || br.trace.startSolid && revBr.trace.startSolid;
			
			if (revTraceHit || allSolid)
			{
				if (revTraceHit)
				{
					auto new_depth = game::BG_GetSurfacePenetrationDepth(penetrate_type, revBr.depthSurfaceType);

					if (has_fmj)
						new_depth *= perk_bulletPenetrationMultiplier;

					max_depth = std::fmin(max_depth, new_depth);

					if (max_depth <= 0.0f)
						return 0.0f;
				}

				const auto depth = get_bullet_depth(br, revBr, revBp, lastHitPos, allSolid);
				
				bp.damageMultiplier -= depth / max_depth;

				if (bp.damageMultiplier <= 0.0f)
					return 0.0f;

				if (!allSolid)
				{
					if (game::Trace_GetEntityHitId(&br.trace) == trace_entity_num)
						return get_bullet_damage(weapon, br, bp);
				}
			}

			if (!traceHitB)
			{
				return get_bullet_damage(weapon, br, bp);
			}
		}

		if (game::Trace_GetEntityHitId(&br.trace) == trace_entity_num)
			return get_bullet_damage(weapon, br, bp);

		return 0.0f;
	}
}