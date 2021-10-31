#include "dependencies/std_include.hpp"
#include "misc.hpp"

namespace misc
{
	bool no_recoil = true; 
	game::TLSData* sys_get_value = nullptr;
	
	void initialize()
	{
		input::on_key(VK_F2, [] { game::Cbuf_AddText(0, "disconnect"); });
		input::on_key(VK_F3, [] { game::Cbuf_AddText(0, "quit"); });

		scheduler::loop([]()
		{
			sys_get_value = game::Sys_GetTLS(); 
				
			const auto lobby_session = game::LobbySession_GetSession(game::LOBBY_TYPE_GAME);
			const auto party_session = game::LobbySession_GetSession(game::LOBBY_TYPE_PRIVATE);

			if (lobby_session->active || (game::session = party_session, !party_session->active))
				game::session = lobby_session;
		}, scheduler::pipeline::main);

		scheduler::loop(game::on_every_frame, scheduler::pipeline::renderer);

		scheduler::loop([]()
		{
			if (!game::LobbyClientLaunch_IsInGame())
				return;
			
			if (no_recoil)
			{
				game::cl()->cgameKickAngles.clear();
			}
		
		}, scheduler::pipeline::renderer);
	}
}