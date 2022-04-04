#include "dependencies/std_include.hpp"
#include "misc.hpp"

namespace misc
{
	bool no_recoil = true; 
	
	void initialize()
	{
		input::on_key(VK_F2, [] { command::execute("disconnect"); });
		input::on_key(VK_F3, [] { command::execute("quit"); });
		
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