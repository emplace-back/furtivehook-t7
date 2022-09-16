#include "dependencies/std_include.hpp"
#include "lobby_msg.hpp"

namespace events::lobby_msg
{
	bool log_messages = true;

	namespace
	{
		bool handle_voice_packet(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			game::Msg_VoicePacket data{};

			if (!game::LobbyMsgRW_PackageInt(&msg, "lobbytype", &data.lobbyType))
				return true;

			if (!game::LobbyMsgRW_PackageUChar(&msg, "talker", &data.talkerIndex))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "relaybits", &data.relayBits))
				return true;
			
			if (!game::LobbyMsgRW_PackageUShort(&msg, "sizeofvoicedata", &data.sizeofVoiceData))
				return true;

			if (!game::LobbyMsgRW_PackageUChar(&msg, "numpackets", &data.numPacketsInData))
				return true;

			if (data.talkerIndex >= 18)
				return true;

			if (!game::is_valid_lobby_type(data.lobbyType) || data.sizeofVoiceData >= 1198)
			{
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::get_sender_string(from).data());
				return true;
			}

			return false;
		}

		bool handle_join_agreement_request(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			game::Msg_JoinAgreementRequest request{};

			if (!game::MSG_JoinAgreementRequest(&request, &msg))
				return true;

			if (!game::is_valid_lobby_type(request.sourceLobbyType))
			{
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::get_sender_string(from).data());
				return true;
			}

			return false;
		}
		
		bool handle_host_disconnect_client(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			PRINT_MESSAGE("LobbyMSG", "Disconnect prevented from %s", utils::get_sender_string(from).data());
			return true;
		}

		bool handle_client_reliable_data(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			game::Msg_ClientReliableData data{};

			if (!game::LobbyMsgRW_PackageUInt(&msg, "datamask", &data.dataMask))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "lobbytype", &data.lobbyType))
				return true;

			if (!game::is_valid_lobby_type(data.lobbyType))
			{
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::get_sender_string(from).data());
				return true;
			}

			return false;
		}

		bool handle_modified_stats(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			game::Msg_ModifiedStats data{};

			if (!game::LobbyMsgRW_PackageInt(&msg, "statssize", &data.statsSize))
				return true;

			if (static_cast<uint32_t>(data.statsSize) >= 65536)
			{
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::get_sender_string(from).data());
				return true;
			}

			return false;
		}

		bool handle_lobby_host_heartbeat(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			game::Msg_LobbyHostHeartbeat data{};

			if (!game::LobbyMsgRW_PackageInt(&msg, "heartbeatnum", &data.heartbeatNum))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "lobbytype", &data.lobbyType))
				return true;

			if (!game::is_valid_lobby_type(data.lobbyType))
			{
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::get_sender_string(from).data());
				return true;
			}

			return false;
		}

		bool handle_client_content(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			game::Msg_ClientContent data{}; 
			
			if (!game::LobbyMsgRW_PackageUInt(&msg, "datamask", &data.dataMask))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "lobbytype", &data.lobbyType))
				return true;

			if (!game::is_valid_lobby_type(data.lobbyType))
			{
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::get_sender_string(from).data());
				return true;
			}

			return false;
		}

		bool handle_join_request(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			game::Msg_JoinParty data{};

			if (!game::LobbyMsgRW_PackageInt(&msg, "targetlobby", &data.targetLobby))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "sourcelobby", &data.sourceLobby))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "jointype", &data.joinType))
				return true;

			if (!game::LobbyMsgRW_PackageXuid(&msg, "probedxuid", &data.probedXuid))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "playlistid", &data.playlistID))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "playlistver", &data.playlistVersion))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "ffotdver", &data.ffotdVersion))
				return true;

			if (!game::LobbyMsgRW_PackageShort(&msg, "networkmode", &data.networkMode))
				return true;

			if (!game::LobbyMsgRW_PackageUInt(&msg, "netchecksum", &data.netFieldChecksum))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "protocol", &data.protocolVersion))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "changelist", &data.changelist))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "pingband", &data.pingBand))
				return true;

			if (!game::LobbyMsgRW_PackageUInt(&msg, "dlcbits", &data.dlcBits))
				return true;

			if (!game::LobbyMsgRW_PackageUInt64(&msg, "joinnonce", &data.joinNonce))
				return true;

			for (size_t i = std::size(data.chunkStatus); i != 0; --i)
			{
				if (!game::LobbyMsgRW_PackageBool(&msg, "chunk", &data.chunkStatus[i]))
					return true;
			}

			if (!game::LobbyMsgRW_PackageBool(&msg, "isStarterPack", &data.isStarterPack))
				return true;

			if (!game::LobbyMsgRW_PackageString(&msg, "password", data.password, sizeof data.password))
				return true;

			if (!game::LobbyMsgRW_PackageInt(&msg, "membercount", &data.memberCount))
				return true;

			if (!game::is_valid_lobby_type(data.targetLobby) || data.memberCount >= 18)
			{
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::get_sender_string(from).data());
				return true;
			}
			
			return false;
		}

		bool handle_member_info(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			game::Msg_JoinMemberInfo data{};

			if (!game::LobbyMsgRW_PackageGlob(&msg, "serializedadr", &data.serializedAdr.xnaddr, sizeof data.serializedAdr.xnaddr))
				return true;

			if (!game::LobbyMsgRW_PackageUInt64(&msg, "reservationkey", &data.reservationKey))
				return true;
			
			if (!game::LobbyMsgRW_PackageInt(&msg, "lobbytype", &data.targetLobby))
				return true;

			if (!game::is_valid_lobby_type(data.targetLobby))
			{
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::get_sender_string(from).data());
				return true;
			}

			return false;
		}

		bool handle_lobby_type(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			int lobby_type_target{ -1 };
			
			if (!game::LobbyMsgRW_PackageInt(&msg, "lobbytype", &lobby_type_target))
				return true;

			if (!game::is_valid_lobby_type(lobby_type_target))
			{
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::get_sender_string(from).data());
				return true;
			}

			return false;
		}

		using callback = std::function<bool(const game::netadr_t&, game::msg_t&, game::LobbyModule module)>;
		using pair = std::pair<game::LobbyModule, game::MsgType>;
		
		auto& get_callbacks()
		{
			static std::unordered_map<pair, callback> callbacks{};
			return callbacks;
		}

		void on_message(const game::LobbyModule module, const game::MsgType type, const callback& callback)
		{
			get_callbacks()[{ module, type }] = callback;
		}
	}

	bool __fastcall handle_packet(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
	{
		const auto ip_str{ utils::get_sender_string(from) };
		const auto type_name{ game::LobbyTypes_GetMsgTypeName(msg.type) };

		if (log_messages)
		{
			PRINT_LOG("Received lobby message [%i] <%s> from %s", module, type_name, ip_str.data());
		}

		const auto& callbacks = get_callbacks();
		const auto handler = callbacks.find({ module, msg.type });

		if (handler == callbacks.end())
		{
			return false;
		}

		return handler->second(from, msg, module);
	}

	void prep_lobby_msg(game::msg_t* msg, char* buffer, const size_t buf_size, const game::MsgType msg_type)
	{
		msg->init(buffer, buf_size);
		
		msg->packageType = game::PACKAGE_TYPE_WRITE;
		msg->type = msg_type;
		msg->encodeFlags = 0;

		msg->write<uint8_t>(game::MESSAGE_ELEMENT_UINT8);
		msg->write<uint8_t>(msg_type);
		msg->write<uint8_t>(game::MESSAGE_ELEMENT_STRING);
		msg->write_data("sike\0"s);
	}

	std::string build_lobby_msg(const game::LobbyModule module)
	{
		auto data{ ""s };
		const auto header{ 0x4864ui16 };
		data.append(reinterpret_cast<const char*>(&header), sizeof header);
		data.push_back(module);
		data.push_back(-1);
		return data;
	}
	
	bool send_to_client(const game::LobbyModule module, const game::msg_t& msg, const game::netadr_t& netadr, const std::uint64_t xuid)
	{
		auto data{ lobby_msg::build_lobby_msg(module) };
		data.append(reinterpret_cast<const char*>(msg.data), msg.cursize); 
		
		if (xuid)
		{
			return game::send_netchan_message(game::session_data(), netadr, xuid, data);
		}
		else
		{
			return game::oob::send(netadr, "LM\n" + data);
		}
	}

	bool send_to_host(const game::LobbySession* session, const game::msg_t& msg, const game::LobbyModule module)
	{
		if (session == nullptr)
			return false;
		
		return lobby_msg::send_to_client(module, msg, session->host.info.netAdr, session->host.info.xuid);
	}

	void initialize()
	{
		utils::hook::jump(game::base_address + 0x1EF5610, game::LobbyMsgRW_PackageElement);
		
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_PEER_TO_PEER_INFO, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_LOBBY_CLIENT_DISCONNECT, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_LOBBY_CLIENT_HEARTBEAT, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_JOIN_LOBBY, lobby_msg::handle_join_request);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_JOIN_MEMBER_INFO, lobby_msg::handle_member_info);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_LOBBY_MODIFIED_STATS, lobby_msg::handle_modified_stats);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_LOBBY_CLIENT_RELIABLE_DATA, lobby_msg::handle_client_reliable_data);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_LOBBY_CLIENT_CONTENT, lobby_msg::handle_client_content);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_VOICE_PACKET, lobby_msg::handle_voice_packet);

		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_LOBBY_HOST_DISCONNECT, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_LOBBY_HOST_LEAVE_WITH_PARTY, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_LOBBY_MIGRATE_ANNOUNCE_HOST, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_INGAME_MIGRATE_TO, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_INGAME_MIGRATE_NEW_HOST, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_LOBBY_HOST_HEARTBEAT, lobby_msg::handle_lobby_host_heartbeat);
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_LOBBY_HOST_DISCONNECT_CLIENT, lobby_msg::handle_host_disconnect_client);
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_JOIN_AGREEMENT_REQUEST, lobby_msg::handle_join_agreement_request);
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_LOBBY_CLIENT_CONTENT, lobby_msg::handle_client_content);

		lobby_msg::on_message(game::LOBBY_MODULE_PEER_TO_PEER, game::MESSAGE_TYPE_PEER_TO_PEER_CONNECTIVITY_TEST, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_PEER_TO_PEER, game::MESSAGE_TYPE_LOBBY_MIGRATE_TEST, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_PEER_TO_PEER, game::MESSAGE_TYPE_LOBBY_MIGRATE_START, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_PEER_TO_PEER, game::MESSAGE_TYPE_DEMO_STATE, lobby_msg::handle_lobby_type);
		lobby_msg::on_message(game::LOBBY_MODULE_PEER_TO_PEER, game::MESSAGE_TYPE_VOICE_PACKET, lobby_msg::handle_voice_packet);
	}
}