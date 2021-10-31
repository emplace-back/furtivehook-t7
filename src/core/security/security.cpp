#include "dependencies/std_include.hpp"
#include "security.hpp"

namespace security
{
	void initialize()
	{
		for (const auto& handler : game::handlers)
		{
			events::connectionless_packet::on_command(handler.first, [=](const auto& args, const auto& target, const auto&)
			{
				return game::send_unhandled_message(target, args.join());
			});
		}
			
		events::netchan::on_message(game::MESSAGE_TYPE_LOBBY_HOST_DISCONNECT_CLIENT, [=](const auto&, const auto&, const auto&)
		{
			PRINT_MESSAGE("Server attempted to disconnect you.");
			return true;
		}); 
		
		events::netchan::on_message(game::MESSAGE_TYPE_LOBBY_MODIFIED_STATS, [=](const auto& target, auto& msg, const auto& sender_id)
		{
			if (auto stats_size{ 0 }; game::LobbyMsgRW_PackageInt(&msg, "statssize", &stats_size))
			{
				if (stats_size < 0 || stats_size > 65536)
				{
					PRINT_MESSAGE("Exploit attempt caught from %s", utils::string::adr_to_string(&target).data());
					return true;
				}
			}

			return false;
		}); 
		
		events::instant_message::dispatch::on_message('f', [=](const auto& sender_id, const auto&)
		{
			PRINT_LOG("Ignoring friend instant message from (%llu)", sender_id);
			return true;
		});

		events::instant_message::dispatch::on_message('e', [=](const auto& sender_id, const auto&)
		{
			PRINT_LOG("Ignoring remote command instant message from (%llu)", sender_id);
			return true;
		});
	}
}