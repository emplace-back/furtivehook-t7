#include "dependencies/std_include.hpp"
#include "utils.hpp"

namespace utils 
{
	void print_log(const char* msg, ...)
	{
		va_list ap;
		va_start(ap, msg);
		const auto result = utils::string::format(ap, msg);
		va_end(ap);

		const static auto file = utils::string::get_log_file("logs");
		utils::io::write_file(file, "[" + utils::string::data_time(0, false) + "] " + result, true);
	}
	
	std::string get_sender_string(const game::netadr_t& from)
	{
		const auto ip_str{ from.to_xnaddr().to_string(true) };
		const auto session = game::session_data();

		if (const auto client_num = game::find_target_from_addr(session, from); client_num >= 0)
		{
			const auto client = session->clients[client_num].activeClient;
			return utils::string::va("'%s' (%llu) %s", client->fixedClientInfo.gamertag, client->fixedClientInfo.xuid, ip_str.data());
		}
		else
		{
			return ip_str;
		}
	}

	std::string get_chat_message(const std::string& data, const uint64_t sender_id)
	{
		const auto session = game::session_data(); 

		if (const auto client_num = game::LobbySession_GetClientNumByXuid(session, sender_id); client_num >= 0)
		{
			return session->clients[client_num].activeClient->fixedClientInfo.gamertag + ": "s + data;
		}
		else
		{
			return std::to_string(sender_id) + ": " + data;
		}
	}
}