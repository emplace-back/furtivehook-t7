#include "dependencies/std_include.hpp"

namespace game
{
	std::unordered_map<std::string, bool> handlers;
	LobbySession* session = nullptr;
	std::array<scr_string_t, static_cast<std::uint32_t>(bone_tag::num_tags)> bone_tags; 
	dvar_t* com_smoothframes_original = nullptr;
	dvar_t* r_fxShadows_original = nullptr;
	dvar_t* unknown_original = nullptr;

	namespace oob
	{
		void send_connection_test(const netadr_t& to, const Msg_ConnectionTest& message)
		{
			char data[0x20000] = { 0 };
			LobbyMsg lobby_msg;

			LobbyMsgRW_PrepWriteMsg(&lobby_msg, data, sizeof data, MESSAGE_TYPE_PEER_TO_PEER_CONNECTIVITY_TEST);
			LobbyMsgRW_PackageInt(&lobby_msg, "lobbytype", &message.lobbyType);
			LobbyMsgRW_PackageInt(&lobby_msg, "index", &message.clientIndex);

			oob::send_lobby_msg(to, &lobby_msg, LOBBY_MODULE_PEER_TO_PEER);
		}

		void send_connection_test(const netadr_t& to, const size_t client_num)
		{
			if (const auto our_client_num = Party_FindMemberByXUID(LiveUser_GetXuid(0)); our_client_num >= 0)
			{
				PRINT_LOG("Sending connectivity test to '%s' (0x%X)",
					utils::string::adr_to_string(&to).data(),
					(1 << our_client_num) & session->clients[client_num].voiceInfo.connectivityBits);

				send_connection_test(to, { session->type, our_client_num });
			}
		}
		
		void send_lobby_msg(const game::netadr_t& to, const LobbyMsg* lobby_msg, const LobbyModule module)
		{
			char data[0x20000] = { 0 };
			game::msg_t msg;

			game::MSG_Init(&msg, data, sizeof data);
			game::MSG_WriteString(&msg, "LM");
			game::MSG_WriteShort(&msg, 18532);
			game::MSG_WriteByte(&msg, module);
			game::MSG_WriteByte(&msg, session->type);
			game::MSG_WriteData(&msg, lobby_msg->msg.data, lobby_msg->msg.cursize);

			game::NET_OutOfBandData(game::NS_SERVER, to, msg.data, msg.cursize);
		}
		
		bool send(const game::netadr_t& target, const std::string& data, const game::netsrc_t& sock)
		{
			return NET_OutOfBandPrint(sock, target, data.data());
		}

		bool register_remote_addr(const game::XSESSION_INFO* info, netadr_t* addr)
		{
			dwRegisterSecIDAndKey(&info->sessionID, &info->keyExchangeKey);
			
			if (!dwCommonAddrToNetadr(addr, &info->hostAddress, &info->sessionID))
			{
				return false;
			}
			
			return true;
		}
	}
	
	void initialize()
	{
		bone_tags[static_cast<std::uint32_t>(bone_tag::helmet)] = GScr_AllocString("j_helmet");
		bone_tags[static_cast<std::uint32_t>(bone_tag::head_end)] = GScr_AllocString("j_head_end");
		bone_tags[static_cast<std::uint32_t>(bone_tag::head)] = GScr_AllocString("j_head");
		bone_tags[static_cast<std::uint32_t>(bone_tag::neck)] = GScr_AllocString("j_neck");
		bone_tags[static_cast<std::uint32_t>(bone_tag::shoulder_left)] = GScr_AllocString("j_shoulder_le");
		bone_tags[static_cast<std::uint32_t>(bone_tag::shoulder_right)] = GScr_AllocString("j_shoulder_ri");
		bone_tags[static_cast<std::uint32_t>(bone_tag::clavicle_left)] = GScr_AllocString("j_clavicle_le");
		bone_tags[static_cast<std::uint32_t>(bone_tag::clavicle_right)] = GScr_AllocString("j_clavicle_ri");
		bone_tags[static_cast<std::uint32_t>(bone_tag::spineupper)] = GScr_AllocString("j_spineupper");
		bone_tags[static_cast<std::uint32_t>(bone_tag::spine)] = GScr_AllocString("j_spine4");
		bone_tags[static_cast<std::uint32_t>(bone_tag::spinelower)] = GScr_AllocString("j_spinelower");
		bone_tags[static_cast<std::uint32_t>(bone_tag::hip_left)] = GScr_AllocString("j_hip_le");
		bone_tags[static_cast<std::uint32_t>(bone_tag::hip_right)] = GScr_AllocString("j_hip_ri");
		bone_tags[static_cast<std::uint32_t>(bone_tag::knee_left)] = GScr_AllocString("j_knee_le");
		bone_tags[static_cast<std::uint32_t>(bone_tag::knee_right)] = GScr_AllocString("j_knee_ri");
		bone_tags[static_cast<std::uint32_t>(bone_tag::wrist_left)] = GScr_AllocString("j_wrist_le");
		bone_tags[static_cast<std::uint32_t>(bone_tag::wrist_right)] = GScr_AllocString("j_wrist_ri");
		bone_tags[static_cast<std::uint32_t>(bone_tag::wrist_twist_left)] = GScr_AllocString("j_wristtwist_le");
		bone_tags[static_cast<std::uint32_t>(bone_tag::wrist_twist_right)] = GScr_AllocString("j_wristtwist_ri");
		bone_tags[static_cast<std::uint32_t>(bone_tag::elbow_left)] = GScr_AllocString("j_elbow_le");
		bone_tags[static_cast<std::uint32_t>(bone_tag::elbow_right)] = GScr_AllocString("j_elbow_ri");
		bone_tags[static_cast<std::uint32_t>(bone_tag::elbow_buldge_left)] = GScr_AllocString("j_elbow_bulge_le");
		bone_tags[static_cast<std::uint32_t>(bone_tag::elbow_buldge_right)] = GScr_AllocString("j_elbow_bulge_ri");
		bone_tags[static_cast<std::uint32_t>(bone_tag::ankle_left)] = GScr_AllocString("j_ankle_le");
		bone_tags[static_cast<std::uint32_t>(bone_tag::ankle_right)] = GScr_AllocString("j_ankle_ri");
		bone_tags[static_cast<std::uint32_t>(bone_tag::ball_left)] = GScr_AllocString("j_ball_le");
		bone_tags[static_cast<std::uint32_t>(bone_tag::ball_right)] = GScr_AllocString("j_ball_ri");
		bone_tags[static_cast<std::uint32_t>(bone_tag::mainroot)] = GScr_AllocString("j_mainroot"); 
		
		handlers["mstart"];
		handlers["vt"];
		handlers["relay"];
		handlers["requeststats"];
		handlers["print"];
		handlers["echo"]; 
		handlers["statusResponse"];
		handlers["connectResponseMigration"];
		handlers["rcon"];
		handlers["RA"];

		com_smoothframes_original = *reinterpret_cast<dvar_t**>(base_address + 0x168EEAC0);
		r_fxShadows_original = *reinterpret_cast<dvar_t**>(base_address + 0xAE96C40);
		unknown_original = *reinterpret_cast<dvar_t**>(base_address + 0x53DD160);
		
		rendering::initialize();
		scheduler::initialize();
		exception::initialize();
		events::connectionless_packet::initialize();
		events::instant_message::initialize();
		events::lobby_msg::initialize();
		security::initialize();
		security::iat::initialize();
		misc::initialize();
		steam::initialize();

		PRINT_LOG("Initialized!");
	}

	bool send_unhandled_netchan_message(const game::netadr_t& from, const game::LobbyMsg& lobby_msg, const std::uint64_t sender_id)
	{
		if (const auto client_num = find_target_from_addr(from); client_num >= 0)
		{
			const auto target_client = session->clients[client_num].activeClient;

			PRINT_LOG("Ignoring netchan message [%i] '%s' from '%s' (%llu) %s",
				lobby_msg.type,
				LobbyTypes_GetMsgTypeName(lobby_msg.type),
				target_client->fixedClientInfo.gamertag,
				sender_id,
				utils::string::adr_to_string(&from).data());
		}
		else
		{
			PRINT_LOG("Ignoring netchan message [%i] '%s' from (%llu) %s",
				lobby_msg.type,
				LobbyTypes_GetMsgTypeName(lobby_msg.type),
				sender_id,
				utils::string::adr_to_string(&from).data());
		}

		return true;
	}
	
	bool send_unhandled_message(const netadr_t& from, const std::string& command)
	{
		if (const auto client_num = find_target_from_addr(from); client_num >= 0)
		{
			const auto target_client = session->clients[client_num].activeClient;

			PRINT_LOG("Ignoring OOB '%s' from '%s' (%llu) %s",
				command.data(),
				target_client->fixedClientInfo.gamertag,
				target_client->fixedClientInfo.xuid,
				utils::string::adr_to_string(&from).data());
		}
		else
		{
			PRINT_LOG("Ignoring OOB '%s' from %s",
				command.data(),
				utils::string::adr_to_string(&from).data());
		}

		return true;
	}

	std::string get_oob_command(const game::netadr_t& from)
	{
		if (const auto client_num = game::find_target_from_addr(from); client_num >= 0)
		{
			const auto target_client = session->clients[client_num].activeClient;

			return utils::string::va("Received OOB '%s' from '%s' (%llu) %s",
				command::args.join().data(),
				target_client->fixedClientInfo.gamertag,
				target_client->fixedClientInfo.xuid,
				utils::string::adr_to_string(&from).data());
		}
		
		return utils::string::va("Received OOB '%s' from %s",
			command::args.join().data(),
			utils::string::adr_to_string(&from).data());
	}

	void bold_game_message(const char* msg, ...)
	{
		char buffer[2048];

		va_list ap;
		va_start(ap, msg);

		vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, msg, ap);

		va_end(ap);

		CG_BoldGameMessageCenter(0, buffer, 0);
	}

	game::netadr_t LobbySession_GetNetAddr(const game::SessionClient *sessionClient)
	{
		if (sessionClient && sessionClient->activeClient)
		{
			const auto session_info = &sessionClient->activeClient->sessionInfo[session->type];

			if (session_info->inSession)
			{
				return session_info->netAdr;
			}
		}

		return {};
	}

	bool NET_CompareXNAddr(const XNADDR *a, const XNADDR *b)
	{
		return std::memcmp(a, b, sizeof XNADDR) == 0;
	}
	
	bool NET_CompareXNAddr(const XSESSION_INFO *a, const XSESSION_INFO *b)
	{
		return NET_CompareXNAddr(&a->hostAddress, &b->hostAddress);
	}
	
	int find_target_from_addr(const netadr_t& from)
	{
		for (auto i = 0u; i != 18u; ++i)
		{
			const auto netadr = LobbySession_GetNetAddr(&session->clients[i]);
			
			if (game::NET_CompareAdr(from, netadr))
				return i;
		}

		return -1;
	}

	int Party_FindMemberByXUID(const std::uint64_t player)
	{
		if (session == nullptr)
			return false;

		return game::LobbySession_GetClientNumByXuid(session->type, player);
	}

	bool can_connect_to_player(const size_t client_num, const size_t target_xuid)
	{
		if (game::LiveUser_GetXuid(0) == target_xuid)
			return true;
		
		if (const auto our_client_num = game::Party_FindMemberByXUID(game::LiveUser_GetXuid(0)); our_client_num >= 0)
		{
			return (1 << our_client_num) & session->clients[client_num].voiceInfo.connectivityBits;
		}

		return false;
	}

	game::XSESSION_INFO get_session_info(const game::InfoResponseLobby& lobby)
	{
		game::XSESSION_INFO sess_info{};
		sess_info.sessionID = lobby.secId;
		sess_info.keyExchangeKey = lobby.secKey;
		sess_info.hostAddress = lobby.serializedAdr.xnaddr;

		return sess_info;
	}

	void on_every_frame()
	{
		aimbot::run(&game::cg()->predictedPlayerState);
		esp::visuals();
	}

	bool is_enemy(const int client_num)
	{
		if (cg()->clients[client_num].team == TEAM_FREE)
		{
			return true;
		}

		return cg()->clients[client_num].team != cg()->clients[cg()->predictedPlayerState.clientNum].team;
	}

	Vec2 get_screen_pos(const Vec3& world_pos)
	{
		Vec2 out{};

		if (const auto screen_pos = spoof_call::call(game::CG_WorldPosToScreenPos, 0u, &world_pos, &out); screen_pos)
		{
			return out;
		}

		return {};
	}

	Vec2 get_scale(const centity_t* cent, const float pos)
	{
		Vec2 scale{};

		if (cent->nextState.lerp.eFlags & 0x08)
		{
			scale.set(pos / 1.5f, pos / 2.0f);
		}

		else if (cent->nextState.lerp.eFlags & 0x04)
		{
			scale.set(pos / 2.0f, pos / 2.0f);
		}

		else
		{
			scale.set(pos / 2.0f, pos / 1.5f);
		}

		scale *= 2.0f;
		return scale;
	}
	
	Vec3 get_top_position(const centity_t* cent)
	{
		auto origin = cent->pose.origin;
		origin.z += 55.0f;

		return origin;
	}

	bool AimTarget_GetTagPos(const game::centity_t* cent, const scr_string_t& tag_name, Vec3* end)
	{
		if (const auto dobj = game::Com_GetClientDObj(cent->nextState.number, 0); dobj)
		{
			return CG_DObjGetWorldTagPos(&cent->pose, dobj, tag_name, end);
		}

		return false;
	}

	bool CG_GetPlayerViewOrigin(const game::playerState_s* ps, Vec3* view_origin)
	{
		const static auto CG_GetPlayerViewOrigin = reinterpret_cast<bool(__fastcall*)(LocalClientNum_t, const playerState_s*, Vec3*)>(base_address + 0x11EF4C0); 
		return spoof_call::call(CG_GetPlayerViewOrigin, 0u, ps, view_origin);
	}

	void adjust_user_cmd_movement(usercmd_s* cmd_old, const float angle, const float old_angle)
	{
		const auto delta_view = DEG2RAD(angle - old_angle);

		cmd_old->forwardmove = ClampChar(static_cast<int>(std::cosf(delta_view) * cmd_old->forwardmove - std::sinf(delta_view) * cmd_old->rightmove));
		cmd_old->rightmove = ClampChar(static_cast<int>(std::sinf(delta_view) * cmd_old->forwardmove + std::cosf(delta_view) * cmd_old->rightmove));
	}

	float get_weapon_damage_range(const Weapon& weapon)
	{
		if (const auto weap_def = BG_GetWeaponDef(weapon); weap_def->weap_class() == WEAPCLASS_SPREAD || weap_def->weap_class() == WEAPCLASS_PISTOL_SPREAD)
		{
			return BG_GetMinDamageRangeScaled(weapon) * BG_GetMultishotBaseMinDamageRangeScaled(weapon);
		}

		return sv_bullet_range->current.value;
	}

	bool CG_BulletTrace(game::BulletTraceResults* br, game::BulletFireParams* bp, const int attacker_entity_num, int lastSurfaceType)
	{
		const static auto CG_BulletTrace = reinterpret_cast<bool(__fastcall*)(LocalClientNum_t, game::BulletFireParams *, game::BulletTraceResults*, const game::Weapon, int, int)>(game::base_address + 0x1168BA0);
		return CG_BulletTrace(0, bp, br, {}, attacker_entity_num, lastSurfaceType);
	}

	bool is_valid_lobby_type(LobbyMsg* msg)
	{
		if (auto lobby_type{ 0 }; game::LobbyMsgRW_PackageInt(msg, "lobbytype", &lobby_type))
		{
			if (lobby_type < game::LOBBY_TYPE_PRIVATE || lobby_type >= game::LOBBY_TYPE_TRANSITION)
			{
				return false;
			}
		}

		return true;
	}

	bool is_valid_lobby_type(const int lobby_type)
	{
		if (lobby_type < game::LOBBY_TYPE_PRIVATE || lobby_type >= game::LOBBY_TYPE_TRANSITION)
		{
			return false;
		}

		return true;
	}
	
	bool MSG_JoinMemberInfo(Msg_JoinMemberInfo* thisptr, game::LobbyMsg* msg)
	{
		return game::LobbyMsgRW_PackageGlob(msg, "serializedadr", &thisptr->serializedAdr, sizeof thisptr->serializedAdr)
			&& game::LobbyMsgRW_PackageUInt64(msg, "reservationkey", &thisptr->reservationKey)
			&& game::LobbyMsgRW_PackageInt(msg, "lobbytype", &thisptr->targetLobby);
	}
	
	bool MSG_LobbyHostHeartbeat(Msg_LobbyHostHeartbeat* thisptr, LobbyMsg* msg)
	{
		return LobbyMsgRW_PackageInt(msg, "heartbeatnum", &thisptr->heartbeatNum)
			&& LobbyMsgRW_PackageInt(msg, "lobbytype", &thisptr->lobbyType)
			&& LobbyMsgRW_PackageInt(msg, "lasthosttimems", &thisptr->migrateInfo.lasthostTimeMS);
	}

	connstate_t CL_GetLocalClientConnectionState(/*LocalClientNum_t localClientNum*/) {
		return *reinterpret_cast<connstate_t*>(base_address + 0x53D9BC8);
	}

	bool in_game()
	{
		if (game::cg() == nullptr)
			return false;

		return game::LobbyClientLaunch_IsInGame() && cg()->clients[cg()->predictedPlayerState.clientNum].infoValid;
	}

	bool is_valid_target(const int client_num)
	{
		if (client_num >= 0 && client_num < 18)
			return session->clients[client_num].activeClient;

		return false;
	}
}