#include "dependencies/std_include.hpp"
#include "utils.hpp"

namespace utils 
{
	std::string get_sender_string(const game::netadr_t & from)
	{
		game::XNADDR xn;
		game::dwNetadrToCommonAddr(from, &xn, sizeof xn, nullptr);

		const auto ip_str{ xn.to_string(true) };
		const auto session = game::session;

		if (const auto client_num = game::find_target_from_addr(from); client_num >= 0)
		{
			const auto client = session->clients[client_num].activeClient;
			return utils::string::va("'%s' (%llu) %s", client->fixedClientInfo.gamertag, client->fixedClientInfo.xuid, ip_str.data());
		}
		else
		{
			return utils::string::va("%s", ip_str.data());
		}
	}
}