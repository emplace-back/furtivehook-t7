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
		
		events::lobby_msg::on_message(game::LOBBY_MODULE_HOST, [=](const auto& target, auto& msg, const auto& sender_id)
		{
			if (msg.type == game::MESSAGE_TYPE_PEER_TO_PEER_INFO
				|| msg.type == game::MESSAGE_TYPE_LOBBY_CLIENT_HEARTBEAT
				|| msg.type == game::MESSAGE_TYPE_LOBBY_CLIENT_DISCONNECT
				|| msg.type == game::MESSAGE_TYPE_VOICE_PACKET)
			{
				if (!game::is_valid_lobby_type(&msg))
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}
			}
			else if (msg.type == game::MESSAGE_TYPE_JOIN_LOBBY)
			{
				game::Msg_JoinParty request{};
				
				if (!game::MSG_JoinParty(&request, &msg))
					return false;

				if (!game::is_valid_lobby_type(request.targetLobby) || !game::is_valid_lobby_type(request.sourceLobby))
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}

				if (request.memberCount < 0 || request.memberCount > 17)
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}
			}
			else if (msg.type == game::MESSAGE_TYPE_JOIN_MEMBER_INFO)
			{
				game::Msg_JoinMemberInfo member_info{};

				if (!game::MSG_JoinMemberInfo(&member_info, &msg))
					return false;

				if (!game::is_valid_lobby_type(member_info.targetLobby))
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}
			}
			else if (msg.type == game::MESSAGE_TYPE_LOBBY_CLIENT_RELIABLE_DATA)
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
			else if (msg.type == game::MESSAGE_TYPE_LOBBY_MODIFIED_STATS)
			{
				if (auto stats_size{ 0 }; game::LobbyMsgRW_PackageInt(&msg, "statssize", &stats_size))
				{
					if (stats_size < 0 || stats_size > 65536)
					{
						PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
						return true;
					}
				}
			}

			return false;
		}); 

		events::lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, [=](const auto& target, auto& msg, const auto& sender_id)
		{
			if (msg.type == game::MESSAGE_TYPE_LOBBY_STATE_PRIVATE
				|| msg.type == game::MESSAGE_TYPE_LOBBY_STATE_GAME
				|| msg.type == game::MESSAGE_TYPE_LOBBY_STATE_TRANSITION)
			{
				game::Msg_LobbyState state{};
				
				if (!game::MSG_LobbyState(&state, &msg))
					return false;
					
				if (!game::is_valid_lobby_type(state.lobbyType))
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}

				if (state.clientCount < 0 || state.clientCount > 17)
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}
			}
			else if (msg.type == game::MESSAGE_TYPE_LOBBY_HOST_DISCONNECT
				|| msg.type == game::MESSAGE_TYPE_LOBBY_HOST_LEAVE_WITH_PARTY
				|| msg.type == game::MESSAGE_TYPE_LOBBY_MIGRATE_ANNOUNCE_HOST
				|| msg.type == game::MESSAGE_TYPE_INGAME_MIGRATE_TO
				|| msg.type == game::MESSAGE_TYPE_INGAME_MIGRATE_NEW_HOST)
			{
				if (!game::is_valid_lobby_type(&msg))
				{
					PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
					return true;
				}
			}
			else if (msg.type == game::MESSAGE_TYPE_LOBBY_HOST_HEARTBEAT)
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
		}); 
		
		events::lobby_msg::on_message(game::LOBBY_MODULE_PEER_TO_PEER, [=](const auto& target, auto& msg, const auto& sender_id)
		{
			if (!game::is_valid_lobby_type(&msg))
			{
				PRINT_MESSAGE("Crash attempt caught from (%llu) %s", sender_id, utils::string::adr_to_string(&target).data());
				return true;
			}

			return false;
		});
		
		events::instant_message::dispatch::on_message('f', [=](const auto& sender_id, auto& msg)
		{
			game::JoinSessionMessage message{}; 
				
			if (msg.cursize - msg.readcount != sizeof message)
			{
				PRINT_MESSAGE("Popup attempt caught from (%llu)", sender_id);
				return true;
			}
				
			game::MSG_ReadData(&msg, &message, sizeof message);

			if (message.mType == game::JOIN_REQUEST)
			{
				PRINT_MESSAGE("Crash attempt caught from (%llu)", sender_id);
				return true;
			}

			PRINT_MESSAGE("Popup attempt caught from (%llu)", sender_id);
			return true;
		});

		events::instant_message::dispatch::on_message('m', [=](const auto& sender_id, const auto&)
		{
			PRINT_LOG("Ignoring 'm' instant message from (%llu)", sender_id);
			return true;
		}); 
		
		events::instant_message::dispatch::on_message('e', [=](const auto& sender_id, const auto&)
		{
			PRINT_LOG("Ignoring remote command instant message from (%llu)", sender_id);
			return true;
		});
	}
}