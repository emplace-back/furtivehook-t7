#include "dependencies/std_include.hpp"
#include "misc.hpp"

namespace misc
{
	bool no_recoil = true; 

	int __stdcall message_box(HWND window, const char* text, const char* caption, int type)
	{
		const auto ret_address{ reinterpret_cast<uintptr_t>(_ReturnAddress()) };

		if (ret_address == OFFSET(0x7FF6C75FE53A)) // Sys_CheckCrashOrRerun
		{
			return IDNO;
		}

		return MessageBoxA(window, text, caption, type);
	}
	
	void initialize()
	{
		utils::hook::iat("user32.dll", "MessageBoxA", message_box);
		
		input::on_key(VK_F2, [] { command::execute("disconnect"); });
		input::on_key(VK_F3, [] { command::execute("quit"); });

		scheduler::once([]()
		{
			utils::hook::return_value(OFFSET(0x7FF6C7162CC8), std::numeric_limits<int>::max()); // Loot_GetItemQuantity
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