#include "dependencies/std_include.hpp"
#include "security.hpp"

namespace security
{
	void initialize()
	{
		iat::initialize();
		
		/*
		events::lobby_msg::on_message(game::LOBBY_MODULE_HOST, [=](const auto& target, auto& msg, const auto& sender_id)
		{
			if (msg.type == game::MESSAGE_TYPE_LOBBY_CLIENT_RELIABLE_DATA)
			{
				game::Msg_ClientReliableData data{};

				if (!game::MSG_ClientReliableData(&data, &msg))
					return false;

				if (!game::is_valid_lobby_type(data.lobbyType))
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}
			}

			return false;
		}); 

		events::lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, [=](const auto& target, auto& msg)
		{
			if (msg.type == game::MESSAGE_TYPE_LOBBY_HOST_HEARTBEAT)
			{
				game::Msg_LobbyHostHeartbeat heartbeat{};

				if (!game::MSG_LobbyHostHeartbeat(&heartbeat, &msg))
				{
					PRINT_MESSAGE("Received invalid message from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}

				if (!game::is_valid_lobby_type(heartbeat.lobbyType))
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}
				
				if (!game::LobbyMsgRW_PackageArrayStart(&msg, "nomineelist"))
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}
			}
			else if (msg.type == game::MESSAGE_TYPE_LOBBY_HOST_DISCONNECT_CLIENT)
			{
				PRINT_MESSAGE("Disconnect prevented from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
				return true;
			}
			else if (msg.type == game::MESSAGE_TYPE_JOIN_AGREEMENT_REQUEST)
			{
				game::Msg_JoinAgreementRequest request{};

				if (!game::MSG_JoinAgreementRequest(&request, &msg))
					return false;

				if (!game::is_valid_lobby_type(request.sourceLobbyType) || !game::is_valid_lobby_type(request.destinationLobbyType))
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}
			}
			else if (msg.type == game::MESSAGE_TYPE_LOBBY_CLIENT_CONTENT)
			{
				game::Msg_ClientContent content{};

				if (!game::MSG_ClientContent(&content, &msg))
					return false;

				if (!game::is_valid_lobby_type(content.lobbyType))
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}

				if (content.compressedBufferSize > sizeof content.clientContentData)
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}
			}

			return false;
		}); */
	}
}