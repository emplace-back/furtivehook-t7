#include "dependencies/std_include.hpp"

namespace game
{
	std::array<scr_string_t, static_cast<std::uint32_t>(bone_tag::num_tags)> bone_tags; 

	namespace oob
	{
		bool send(const netadr_t& target, const std::string& data)
		{
			return NET_OutOfBandData(NS_SERVER, target, data.data(), data.size());
		}

		bool register_remote_addr(const XSESSION_INFO* info, netadr_t* addr)
		{
			dwRegisterSecIDAndKey(&info->sessionID, &info->keyExchangeKey);
			
			if (!dwCommonAddrToNetadr(addr, &info->hostAddress, &info->sessionID))
			{
				return false;
			}
			
			return true;
		}

		bool register_remote_addr(const InfoResponseLobby& lobby, netadr_t* addr)
		{
			const auto session_info{ get_session_info(lobby) };
			return register_remote_addr(&session_info, addr);
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

		std::thread{ exception::initialize }.detach();

		steam::initialize();
		command::initialize();
		rendering::initialize();

		PRINT_LOG("Initialized!");
	}

	bool in_game()
	{
		if (cg() == nullptr)
			return false;

		return LobbyClientLaunch_IsInGame() && cg()->clients[cg()->clientNum].infoValid;
	}

	void on_every_frame()
	{
		if (in_game())
		{
			aimbot::run(&cg()->predictedPlayerState); 
		}

		esp::visuals();
	}

	bool is_enemy(const int client_num)
	{
		if (cg()->clients[client_num].team == TEAM_FREE)
		{
			return true;
		}

		return cg()->clients[client_num].team != cg()->clients[cg()->clientNum].team;
	}
	
	int find_target_from_addr(const LobbySession* session, const netadr_t& from)
	{
		if (session == nullptr)
			return -1;
		
		for (size_t i = 0; i < 18; ++i)
		{
			if (const auto client = session->clients[i].activeClient; client)
			{
				const auto netadr = get_session_netadr(session, client);
				if (NET_CompareAdr(from, netadr))
					return i;
			}
		}

		return -1;
	}

	bool can_connect_to_player(const LobbySession* session, const size_t client_num, const size_t target_xuid)
	{
		if (const auto our_client_num = LobbySession_GetClientNumByXuid(session, LiveUser_GetXuid(0));
			our_client_num >= 0 && client_num < std::size(session->clients))
		{
			if (our_client_num == client_num)
				return true;
			
			return (1 << our_client_num) & session->clients[client_num].voiceInfo.connectivityBits;
		}

		return false;
	}

	XSESSION_INFO get_session_info(const InfoResponseLobby& lobby)
	{
		XSESSION_INFO sess_info{};
		sess_info.sessionID = lobby.secId;
		sess_info.keyExchangeKey = lobby.secKey;
		sess_info.hostAddress = lobby.serializedAdr.xnaddr;

		return sess_info;
	}

	bool CG_WorldPosToScreenPos(const Vec3* pos, Vec2* out)
	{
		const static auto CG_WorldPosToScreenPos = reinterpret_cast<bool(*)(LocalClientNum_t, const Vec3* worldPos, Vec2* outScreenPos)>(base_address + 0x573140); 
		return spoof_call::call(CG_WorldPosToScreenPos, 0u, pos, out);
	}

	Vec2 get_screen_pos(const Vec3& world_pos)
	{
		Vec2 out{};

		if (const auto screen_pos = CG_WorldPosToScreenPos(&world_pos, &out); screen_pos)
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

	bool AimTarget_GetTagPos(const centity_t* cent, const scr_string_t& tag_name, Vec3* end)
	{
		if (cent == nullptr)
		{
			return false;
		}

		const auto dobj = Com_GetClientDObj(cent->nextState.number, 0);
		if (!dobj)
		{
			return false;
		}

		return CG_DObjGetWorldTagPos(&cent->pose, dobj, tag_name, end);
	}

	bool CG_GetPlayerViewOrigin(const playerState_s* ps, Vec3* view_origin)
	{
		if (ps == nullptr)
			return false;
		
		const static auto CG_GetPlayerViewOrigin = reinterpret_cast<bool(*)(LocalClientNum_t, const playerState_s*, Vec3*, uint32_t)>(base_address + 0x11EF4C0); 
		return spoof_call::call(CG_GetPlayerViewOrigin, 0u, ps, view_origin, 0u);
	}

	void adjust_user_cmd_movement(usercmd_s* cmd_old, const float angle, const float old_angle)
	{
		const auto delta_view = DEG2RAD(angle - old_angle);

		cmd_old->forwardmove = ClampChar(static_cast<int>(std::cosf(delta_view) * cmd_old->forwardmove - std::sinf(delta_view) * cmd_old->rightmove));
		cmd_old->rightmove = ClampChar(static_cast<int>(std::sinf(delta_view) * cmd_old->forwardmove + std::cosf(delta_view) * cmd_old->rightmove));
	}

	void adjust_user_cmd_movement(const usercmd_s* cmd, usercmd_s* cmd_old, const float yaw)
	{
		if (cmd->forwardmove || cmd->rightmove)
		{
			const auto move_angle = math::angle_normalize_360(RAD2DEG(std::atan2(-cmd->rightmove / 127.0f, cmd->forwardmove / 127.0f)));
			const auto delta_angle = math::angle_normalize_360(yaw - SHORT2ANGLE(cmd->angles[1]));
			const auto dest_angle = DEG2RAD(math::angle_normalize_360(move_angle - delta_angle));
			auto forwardmove_ratio = std::cosf(dest_angle);
			auto rightmove_ratio = -std::sinf(dest_angle);

			const auto abs_forwardmove_ratio = std::abs(forwardmove_ratio);
			const auto abs_rightmove_ratio = std::abs(rightmove_ratio);

			if (abs_forwardmove_ratio < abs_rightmove_ratio)
			{
				forwardmove_ratio *= 1.0f / abs_rightmove_ratio;
				rightmove_ratio = rightmove_ratio > 0.0f ? 1.0f : -1.0f;
			}
			else if (abs_forwardmove_ratio > abs_rightmove_ratio)
			{
				rightmove_ratio *= 1.0f / abs_forwardmove_ratio;
				forwardmove_ratio = forwardmove_ratio > 0.0f ? 1.0f : -1.0f;
			}
			else
			{
				forwardmove_ratio = 1.0f;
				rightmove_ratio = 1.0f;
			}

			cmd_old->forwardmove = ClampChar(static_cast<int>(forwardmove_ratio * std::numeric_limits<char>::max()));
			cmd_old->rightmove = ClampChar(static_cast<int>(rightmove_ratio * std::numeric_limits<char>::max()));
		}
	}

	float get_weapon_damage_range(const Weapon& weapon)
	{
		const auto weapon_def{ BG_GetWeaponDef(weapon) };

		if (weapon_def == nullptr)
		{
			return sv_bullet_range->current.value;
		}

		if (weapon_def->weapClass == WEAPCLASS_SPREAD || weapon_def->weapClass == WEAPCLASS_PISTOL_SPREAD)
		{
			return BG_GetMinDamageRangeScaled(weapon) * BG_GetMultishotBaseMinDamageRangeScaled(weapon);
		}

		return sv_bullet_range->current.value;
	}

	bool CG_BulletTrace(BulletTraceResults* br, BulletFireParams* bp, const int attacker_entity_num, int lastSurfaceType)
	{
		return reinterpret_cast<bool(*)(LocalClientNum_t, BulletFireParams *, BulletTraceResults*, const Weapon, int, int)>(base_address + 0x1168BA0)(0, bp, br, {}, attacker_entity_num, lastSurfaceType);
	}

	bool send_instant_message(const std::vector<std::uint64_t>& recipients, const std::uint8_t type, const void* message, const std::uint32_t message_size)
	{
		if (!Live_IsUserSignedInToDemonware(0))
			return false; 
		
		return dwInstantSendMessage(0, recipients.data(), recipients.size(), type, message, message_size);
	}

	bool send_instant_message(const std::vector<std::uint64_t>& recipients, const std::uint8_t type, const std::string& data)
	{
		return send_instant_message(recipients, type, data.data(), data.size());
	}
	
	bool send_instant_message(const std::vector<std::uint64_t>& recipients, const std::uint8_t type, const msg_t& msg)
	{
		return send_instant_message(recipients, type, msg.data, msg.cursize);
	}

	bool send_netchan_message(const game::LobbySession* session, const netadr_t& netadr, const std::uint64_t xuid, const std::string& data)
	{
		if (session == nullptr)
			return false;
		
		const auto channel = LobbyNetChan_GetLobbyChannel(session->type, LOBBY_CHANNEL_UNRELIABLE);
		return Netchan_SendMessage(0, channel, NETCHAN_UNRELIABLE, data.data(), data.size(), xuid, netadr, nullptr);
	}

	const char* LobbyTypes_GetMsgTypeName(const MsgType type)
	{
		if (type < MESSAGE_TYPE_INFO_REQUEST || type > MESSAGE_TYPE_DEMO_STATE)
		{
			return "Invalid";
		}

		return lobbyMsgName[type];
	}

	int LobbySession_GetClientNumByXuid(const game::LobbySession* session, const std::uint64_t xuid)
	{
		if (session == nullptr)
			return -1;
		
		for (size_t i = 0; i < 18; ++i)
		{
			const auto client = session->clients[i].activeClient;

			if (client && client->fixedClientInfo.xuid == xuid)
				return i;
		}

		return -1;
	}

	bool is_valid_lobby_type(const int lobby_type)
	{
		return lobby_type >= LOBBY_TYPE_PRIVATE && lobby_type < LOBBY_TYPE_TRANSITION;
	}
	
	void enum_assets(const XAssetType type, const std::function<void(XAssetHeader)>& callback, const bool includeOverride)
	{
		DB_EnumXAssets(type, static_cast<void(*)(XAssetHeader, void*)>([](XAssetHeader header, void* data)
		{
			const auto& cb = *static_cast<const std::function<void(XAssetHeader)>*>(data);
			cb(header);
		}), &callback, includeOverride);
	}

	bool LobbyMsgRW_PackageElement(msg_t* msg, bool add_element)
	{
		switch (msg->packageType)
		{
		case PACKAGE_TYPE_NONE:
			return false;
		case PACKAGE_TYPE_WRITE:
			if (!add_element)
			{
				msg->write(14ui8);
				msg->encodeFlags = 0;
				return false;
			}

			msg->write(13ui8);
			break;
		case PACKAGE_TYPE_READ:
			const auto result = LobbyMsgRW_IsEndOfArray(msg);
			const auto overflowed = msg->overflowed;
			
			if (result || overflowed)
			{
				if (overflowed)
				{
					utils::toast::add_toast("LobbyMSG", "Message overflowed.");
					PRINT_LOG("Message overflowed [%s]", LobbyTypes_GetMsgTypeName(msg->type));
				}
				
				msg->encodeFlags = 0;
				return false;
			}

			break;
		}

		return !msg->overflowed;
	}

	netadr_t get_session_netadr(const game::LobbySession* session, const ActiveClient* client)
	{
		if (session == nullptr)
			return {};
		
		netadr_t netadr{};
		
		if (client)
		{
			const auto session_info = &client->sessionInfo[session->type];
			
			if (session_info)
			{
				netadr = session_info->netAdr;
			}
		}
		
		return netadr;
	}

	int I_stricmp(const std::string& a, const std::string& b)
	{
		return _strnicmp(a.data(), b.data(), std::numeric_limits<int>::max());
	}

	char* I_strncpyz(char* place, const std::string& string, const size_t length)
	{
		const auto result = std::strncpy(place, string.data(), length);
		place[length - 1] = 0;
		return result;
	}

	std::string get_gametype_on_mapname(const int map_id, const int gametype_id)
	{
		const auto entry = Com_GameInfo_GetMapForIndex(map_id);
		const auto gametype_name = Com_GameInfo_GetGameTypeNameForID(gametype_id);

		if (entry
			&& gametype_name 
			&& *gametype_name)
		{
			char buffer[0x100] = { 0 };
			Com_GameInfo_GetGameTypeOnMapName(gametype_name, entry->name, buffer, sizeof buffer);

			if (buffer)
			{
				return { buffer, sizeof buffer };
			}
		}

		return "";
	}
}