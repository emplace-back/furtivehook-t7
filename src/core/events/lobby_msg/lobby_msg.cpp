#include "dependencies/std_include.hpp"
#include "lobby_msg.hpp"

namespace events::lobby_msg
{
	utils::hook::detour lobby_msg_rw_package_int_hook;
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

			if (!game::is_valid_lobby_type(data.lobbyType) || data.sizeofVoiceData > sizeof(data.voiceData))
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

		bool handle_modified_stats(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			game::Msg_ModifiedStats data{};

			if (!game::LobbyMsgRW_PackageInt(&msg, "statssize", &data.statsSize))
				return true;

			if (static_cast<uint32_t>(data.statsSize) > sizeof(data.statsBuffer))
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

			if (game::call<uint32_t>(0x7FF6C71CB710, data.dataMask) >= 2)
			{
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::get_sender_string(from).data());
				return true;
			}

			return false;
		}

		bool handle_agreement_request(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			if (const auto state = *reinterpret_cast<game::JoinSourceState*>(OFFSET(0x7FF6DAA2C640));
				state != game::JOIN_SOURCE_STATE_WAITING_FOR_AGREEMENT)
			{
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
				if (!game::LobbyMsgRW_PackageUChar(&msg, "chunk", &data.chunkStatus[i]))
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

		using pair = std::pair<game::LobbyModule, game::MsgType>;

		auto& get_callbacks()
		{
			static std::unordered_map<pair, callback> callbacks{};
			return callbacks;
		}

		void lobby_debug_join_state_changed(const char* state, const char* reason)
		{
			PRINT_MESSAGE("LobbyMSG", "Host rejected our join request due to '%s'", game::UI_SafeTranslateString(reason));
			return game::call(0x7FF6C71CEAB0, state, reason);
		}
		
		bool __fastcall lobby_msg_rw_package_int(game::msg_t* msg, const char* key, int* value)
		{
			const auto result = lobby_msg_rw_package_int_hook.call<bool>(msg, key, value);

			if (msg->packageType == game::PACKAGE_TYPE_READ)
			{
				const std::vector<std::pair<std::string, uint32_t>> patches =
				{
					{ "lobbytype", 2 },
					{ "clientcount", 19 },
					{ "votecount", 216 },
					{ "settingssize", 0xC000 },
				};

				const auto result = std::any_of(patches.begin(), patches.end(), [&](const auto& p) { return p.first == utils::string::to_lower(key) && *value >= p.second; });

				if (result)
				{
					PRINT_LOG("Crash attempt caught <%s> with key '%s' of value [%i]", game::LobbyTypes_GetMsgTypeName(msg->type), key, *value);
					msg->overflowed = 1;
					return false;
				}
			}

			return result;
		}

		bool __fastcall handle_packet(const game::LobbyModule module, const game::netadr_t& from, game::msg_t* msg)
		{
			const auto ip_str{ utils::get_sender_string(from) };
			const auto type_name{ game::LobbyTypes_GetMsgTypeName(msg->type) };

			if (log_messages)
			{
				PRINT_LOG("Received lobby message [%i] <%s> from %s", module, type_name, ip_str.data());
			}

			const auto& callbacks = get_callbacks();
			const auto handler = callbacks.find({ module, msg->type });

			if (handler == callbacks.end())
				return false;

			const auto msg_backup = *msg;
			const auto handled = handler->second(from, *msg, module);

			if (handled)
			{
				msg->type = static_cast<game::MsgType>(game::MESSAGE_TYPE_NONE);
			}
			else
			{
				*msg = msg_backup;
			}

			return handled;
		}
	}

	void on_message(const game::LobbyModule module, const game::MsgType type, const callback& callback)
	{
		get_callbacks()[{ module, type }] = callback;
	}

	std::string build_lobby_msg(const game::LobbyModule module)
	{
		auto data{ ""s };
		const auto header{ static_cast<uint16_t>(lobby_msg::PROTOCOL) };
		data.append(reinterpret_cast<const char*>(&header), sizeof header);
		data.push_back(static_cast<uint8_t>(module));
		data.push_back(-1);
		return data;
	}

	bool send_lobby_msg(const game::NetChanMsgType channel, const game::LobbyModule module, const game::msg_t& msg, const game::netadr_t& netadr, const uint64_t src_xuid)
	{
		auto data{ lobby_msg::build_lobby_msg(module) };
		data.append(reinterpret_cast<const char*>(msg.data), msg.cursize);

		//return game::net::netchan::send(channel, data, netadr, 0, src_xuid);
		return game::net::netchan::send2(channel, data, netadr, 0xDEADFA11, src_xuid);
	}

	bool send_lobby_msg(const game::LobbyModule module, const game::msg_t& msg, const game::netadr_t& netadr, const uint64_t src_xuid)
	{
		const auto session = game::session_data();

		if (session == nullptr)
			return false;

		const auto channel = game::call<game::NetChanMsgType>(0x7FF6C71D8F70, session->type, game::LOBBY_CHANNEL_UNRELIABLE);
		return send_lobby_msg(channel, module, msg, netadr);
	}
	
	void initialize()
	{
		const auto handle_packet_internal_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.lea(r8, qword_ptr(rsp, 0x30));
			a.mov(ecx, dword_ptr(rsp, 0xB0));

			a.pushad64();
			a.mov(rdx, rbp);
			a.call_aligned(lobby_msg::handle_packet);
			a.popad64();

			a.jmp(OFFSET(0x7FF6C71D809B));
		}); 
		
		lobby_msg_rw_package_int_hook.create(OFFSET(0x7FF6C71D6720), lobby_msg_rw_package_int);
		
		utils::hook::jump(OFFSET(0x7FF6C71D8094), handle_packet_internal_stub);
		utils::hook::nop(OFFSET(0x7FF6C71D8094) + 5, 2);
		
		utils::hook::jump(OFFSET(0x7FF6C71D6610), game::LobbyMsgRW_PackageElement);
		utils::hook::call(OFFSET(0x7FF6C71C6767), lobby_debug_join_state_changed);

		utils::hook::return_value(OFFSET(0x7FF6C71D6BD0), 0); // LobbyMsgRW_PrintDebugMessage
		utils::hook::return_value(OFFSET(0x7FF6C71D6E90), 0); // LobbyMsgRW_PrintMessage
		
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_JOIN_LOBBY, lobby_msg::handle_join_request);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_LOBBY_MODIFIED_STATS, lobby_msg::handle_modified_stats);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_LOBBY_CLIENT_CONTENT, lobby_msg::handle_client_content);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_VOICE_PACKET, lobby_msg::handle_voice_packet);

		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_LOBBY_HOST_DISCONNECT_CLIENT, lobby_msg::handle_host_disconnect_client);
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_LOBBY_CLIENT_CONTENT, lobby_msg::handle_client_content);
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, game::MESSAGE_TYPE_JOIN_AGREEMENT_REQUEST, lobby_msg::handle_agreement_request);

		lobby_msg::on_message(game::LOBBY_MODULE_PEER_TO_PEER, game::MESSAGE_TYPE_VOICE_PACKET, lobby_msg::handle_voice_packet);
	}
}