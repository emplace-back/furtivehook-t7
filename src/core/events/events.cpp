#include "dependencies/std_include.hpp"
#include "events.hpp"

namespace events
{
	void cg_predict_playerstate()
	{
		auto cmd_cur = game::cl()->user_cmd(game::cl()->cmdNumber);
		auto cmd_old = game::cl()->user_cmd(game::cl()->cmdNumber - 1);
		auto cmd_new = game::cl()->user_cmd(++game::cl()->cmdNumber);

		*cmd_new = *cmd_cur;
		cmd_cur->serverTime -= 1;
		cmd_old->serverTime += 1;

		static std::uint32_t angle_backup[3];
		VECTOR_COPY(cmd_old->angles, angle_backup);
		VECTOR_COPY(angle_backup, cmd_cur->angles);

		static std::uint32_t angle_backup_2[3];
		VECTOR_COPY(cmd_cur->angles, angle_backup_2);
		VECTOR_COPY(angle_backup_2, cmd_new->angles);

		const auto weapon = game::BG_GetViewmodelWeaponIndex(&game::cg()->predictedPlayerState); 
		const auto is_akimbo_gun = game::BG_IsDualWield(weapon);
		const auto is_auto_fire = aimbot::aim_target && aimbot::auto_fire;
		auto button_flag = 0u;

		if (is_auto_fire)
		{
			button_flag |= 0x80000000;

			if (is_akimbo_gun)
				button_flag |= 0x100080;

			cmd_cur->button_bits[0] |= button_flag;
		}

		if (button_flag || !is_auto_fire)
		{
			const auto old_angle = SHORT2ANGLE(cmd_old->angles[1]); 
			
			if (aimbot::aim_target)
			{
				cmd_old->angles[0] += ANGLE2SHORT(aimbot::aim_angles[0]);
				cmd_old->angles[1] += ANGLE2SHORT(aimbot::aim_angles[1]);
				cmd_old->angles[2] += ANGLE2SHORT(aimbot::aim_angles[2]);
			}

			if (nospread::enabled)
			{
				const auto spread_angles = nospread::get_spread_angles(cmd_cur->serverTime, weapon);

				cmd_old->angles[0] += ANGLE2SHORT(spread_angles[0]);
				cmd_old->angles[1] += ANGLE2SHORT(spread_angles[1]);
				cmd_old->angles[2] += ANGLE2SHORT(spread_angles[2]);
			}

			if (aimbot::aim_target && aimbot::silent)
			{
				adjust_user_cmd_movement(cmd_old, SHORT2ANGLE(cmd_old->angles[1]), old_angle);
			}
		}
	}
}