#include "dependencies/std_include.hpp"
#include "misc.hpp"

namespace misc
{
	utils::hook::detour sys_get_tls_hook; 
	
	bool no_recoil = true, prediction = true;
	game::TLSData* tls_data{ nullptr };
	
	BOOL __stdcall set_process_dpi_aware_stub()
	{
		arxan::search_and_patch_integrity_checks();
		security::initialize();
		
		return SetProcessDPIAware();
	}
	
	int __stdcall message_box(HWND window, const char* text, const char* caption, int type)
	{
		const auto ret_address{ reinterpret_cast<uintptr_t>(_ReturnAddress()) };

		if (ret_address == OFFSET(0x7FF6C75FE53A)) // Sys_CheckCrashOrRerun
		{
			return IDNO;
		}

		return MessageBoxA(window, text, caption, type);
	}

	game::TLSData* __fastcall sys_get_tls()
	{
		const auto result = sys_get_tls_hook.call<game::TLSData*>();
		
		if (!result && tls_data)
		{
			return tls_data;
		}

		return result;
	}

	void __fastcall cg_interpolate_entity_position(game::cg_t *cgameGlob, game::centity_t *cent, LocalClientNum_t localClientNum)
	{
		game::call(0x7FF6C5A09E50, cgameGlob, cent, localClientNum);
		
		if (misc::prediction && aimbot::enabled && aimbot::is_valid_target(cent))
		{
			Vec3 origin{}, angles{};

			// calc_entity_origin_and_angles
			game::call(0x7FF6C767D150, 0, cent, &origin, &angles);

			origin[0] = static_cast<float>(GET_SIGN(origin[0]));
			origin[1] = static_cast<float>(GET_SIGN(origin[1]));
			origin[2] = static_cast<float>(GET_SIGN(origin[2]));

			angles[0] = static_cast<float>(GET_SIGN(angles[0]));
			angles[1] = static_cast<float>(GET_SIGN(angles[1]));
			angles[2] = static_cast<float>(GET_SIGN(angles[2]));

			const auto delta = cgameGlob->next_snap()->serverTime - cgameGlob->snap()->serverTime;
			const auto ping = cgameGlob->snap()->ping;

			if (delta)
			{
				cent->pose.origin += origin * (delta * 0.001f);
				cent->pose.angles += angles * (delta * 0.001f);
			}

			cent->pose.origin += origin * (ping * 0.001f);
			cent->pose.angles += angles * (ping * 0.001f);
		}
	}
	
	void initialize()
	{
		utils::hook::iat("user32.dll", "SetProcessDPIAware", set_process_dpi_aware_stub);
		utils::hook::iat("user32.dll", "MessageBoxA", message_box);
		
		input::on_key(VK_F2, [] { command::execute("disconnect"); });
		input::on_key(VK_F3, [] { command::execute("quit"); });

		scheduler::once([=]()
		{
			utils::hook::call(OFFSET(0x7FF6C59FB356), cg_interpolate_entity_position);
			sys_get_tls_hook.create(OFFSET(0x7FF6C7464210), sys_get_tls);
			utils::hook::return_value(OFFSET(0x7FF6C7162C90), std::numeric_limits<int>::max()); // Loot_GetItemQuantity
		}, scheduler::pipeline::main);

		scheduler::once(game::initialize, scheduler::pipeline::main);
		
		scheduler::loop([]()
		{
			for (size_t i = 0; i < 18; ++i)
			{
				const auto session = game::session_data();
				const auto valid = session->clients[i].activeClient;

				if (!valid)
				{
					aimbot::priority_target[i] = false;
					aimbot::ignore_target[i] = false;
				}
			}

			tls_data = game::Sys_GetTLS();
		}, scheduler::pipeline::main);

		scheduler::loop(game::on_every_frame, scheduler::pipeline::renderer); 

		scheduler::loop([]()
		{
			if (!game::in_game())
				return;

			if (!game::centity(game::cg()->clientNum)->is_alive())
				return;
				
			if (no_recoil)
				game::cl()->cgameKickAngles.clear();
		}, scheduler::pipeline::renderer);
	}
}