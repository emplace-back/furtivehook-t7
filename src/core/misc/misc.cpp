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
			const auto lobby_session = game::get_client_session(game::LOBBY_TYPE_GAME);
			const auto party_session = game::get_client_session(game::LOBBY_TYPE_PRIVATE);

			if (lobby_session->active || (game::session = party_session, !party_session->active))
				game::session = lobby_session;

			for (size_t i = 0; i < 18; ++i)
			{
				if (!game::is_valid_target(i))
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