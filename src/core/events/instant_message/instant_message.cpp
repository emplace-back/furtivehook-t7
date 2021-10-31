#include "dependencies/std_include.hpp"
#include "instant_message.hpp"

namespace events::instant_message
{
	void send_popup(const std::uint64_t target_steam_id)
	{
		const game::JoinSessionMessage message{};
		game::dwInstantSendMessage(0, &target_steam_id, 1, 'f', &message, 0);
	}
	
	void send_crash(const std::uint64_t target_steam_id)
	{
		const game::JoinSessionMessage message{ game::JOIN_REQUEST };
		game::dwInstantSendMessage(0, &target_steam_id, 1, 'f', &message, sizeof message);
	}

	void send_info_request(const std::vector<std::uint64_t>& recipients)
	{
		if (!*recipients.data())
		{
			return;
		}

		const game::Msg_InfoRequest message{ 1111111111 };
		game::LobbyMsg_SendInfoRequest(0, recipients.data(), recipients.size(), &message);
	}

	void add_response(friends::friends_t& friends, const response_t& response)
	{
		if (response.host_xuid == friends.steam_id)
		{
			const auto is_same_address{ game::NET_CompareXNAddr(&friends.response.session_info, &response.session_info) };

			if (is_same_address)
			{
				friends.response = response;
				return;
			}

			friends.response = response;
			friends.last_online = response.last_online;
			friends::write_to_friends();

			PRINT_MESSAGE("%s (%s) is online.",
				friends.name.data(),
				utils::string::adr_to_string(&response.session_info.hostAddress).data());
		}
	}

	void add_friend_response(const std::uint32_t nonce, const game::InfoResponseLobby& lobby)
	{
		const auto last_online{ std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() };
		const response_t response{ nonce > 0, lobby.hostXuid, lobby.hostName, lobby.lobbyParams.mainMode, game::get_session_info(lobby), last_online };

		for (auto& friends : friends::friends)
			add_response(friends, response);
	}
	
	namespace dispatch
	{
		std::unordered_map<std::uint8_t, callback> messages;
		bool log_messages = false; 
		
		void on_message(const std::uint8_t command, const callback& callback)
		{
			if (messages.find(command) == messages.end())
			{
				messages[command] = callback;
			}
		}

		bool handle_message(const std::uint64_t sender_id, const char* message, const std::uint32_t message_size)
		{
			if (message_size >= 0x1000)
			{
				PRINT_LOG("Received instant message of invalid size [%u] from (%llu)", message_size, sender_id);
				return true;
			}

			game::msg_t msg{};
			game::MSG_InitReadOnly(&msg, message, message_size);
			game::MSG_BeginReading(&msg);

			auto type = 0ui8;

			if (game::MSG_ReadByte(&msg) == '1')
			{
				type = game::MSG_ReadByte(&msg);
			}

			if (const auto message_func = messages.find(type); message_func != messages.end())
			{
				return message_func->second(sender_id, msg);
			}

			instant_message::dispatch::log_instant_messages(sender_id, message_size, type);
			return false;
		}

		void log_instant_messages(const std::uint64_t sender_id, const std::uint32_t message_size, const std::uint8_t type)
		{
			if (!log_messages)
			{
				return logger::print_log("Received instant message '%c' of size [%u] from (%llu)", type, message_size, sender_id);
			}

			PRINT_LOG("Received instant message '%c' of size [%u] from (%llu)", type, message_size, sender_id);
		}

		void initialize()
		{
			instant_message::dispatch::on_message('h', [=](const auto& sender_id, auto& msg)
			{
				if (const auto size{ msg.cursize - msg.readcount }; size < 2048u)
				{
					char data[2048] = { 0 };
					game::MSG_ReadData(&msg, data, size);

					if (!msg.overflowed)
					{
						game::LobbyMsg lobby_msg{};
						game::LobbyMsgRW_PrepReadData(&lobby_msg, data, size);

						if (lobby_msg.msgType == game::MESSAGE_TYPE_INFO_RESPONSE)
						{
							game::Msg_InfoResponse response{};

							if (!game::MSG_InfoResponse(&response, &lobby_msg))
								return false;

							add_friend_response(response.nonce, response.lobby[0]);
						}
					}
				}

				return false;
			});
		}
	}

	void initialize()
	{
		instant_message::dispatch::initialize();
		
		scheduler::on_dw_initialized([]()
		{
			for (const auto& friends : friends::friends)
			{
				std::vector<std::uint64_t> recipients; 
				recipients.emplace_back(friends.steam_id);

				if (recipients.size() > 18)
					return;

				send_info_request(recipients);
			}

		}, scheduler::pipeline::main, 10s);
	}
}