#include "dependencies/std_include.hpp"
#include "events.hpp"

namespace events
{
	bool prevent_join = true, no_presence = true;
	
	void __cdecl cg_predict_playerstate(LocalClientNum_t localClientNum)
	{
		if(!game::in_game())
			return;
		
		if (!game::centity(game::cg()->clientNum)->is_alive())
			return;
		
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
 
		const auto is_akimbo_gun = game::BG_IsDualWield(cmd_cur->weapon);
		const auto is_auto_fire = aimbot::aim_target && aimbot::auto_fire;
		auto button_flag = 0;

		if (is_auto_fire)
		{
			button_flag |= 0x80000000;

			if (is_akimbo_gun)
				button_flag |= 0x100080;

			cmd_cur->button_bits[0] |= button_flag;
		}

		auto cmd_angles = cmd_old->get_angles();

		if (aimbot::aim_target)
		{
			cmd_angles += aimbot::aim_angles;
			cmd_angles = math::angle_normalize_360(cmd_angles);
		}

		if (nospread::enabled)
		{
			const auto spread_angles = nospread::get_spread_angles(
				&game::cg()->predictedPlayerState, 
				cmd_cur);

			cmd_angles += spread_angles;
			cmd_angles = math::angle_normalize_360(cmd_angles);
		}

		cmd_old->set_angles(cmd_angles);

		if (aimbot::aim_target && aimbot::silent)
		{
			game::adjust_user_cmd_movement(cmd_cur, cmd_old, SHORT2ANGLE(cmd_old->angles[1]));
		}

		return reinterpret_cast<decltype(&cg_predict_playerstate)>(game::base_address + 0x9C2AF0)(localClientNum);
	}
	
	void initialize()
	{
		connectionless_packet::initialize();
		instant_message::initialize();
		lobby_msg::initialize();
		server_command::initialize();
		
		//call(game::base_address + 0x10BA99D, &cg_predict_playerstate);
	}
}