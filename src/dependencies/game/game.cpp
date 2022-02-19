#include "dependencies/std_include.hpp"

namespace game
{
	std::unordered_map<std::string, bool> handlers;
	LobbySession* session = nullptr;
	std::array<scr_string_t, static_cast<std::uint32_t>(bone_tag::num_tags)> bone_tags; 

	namespace oob
	{
		bool send(const game::netadr_t& target, const std::string& data, const game::netsrc_t& sock)
		{
			return NET_OutOfBandData(sock, target, data.data(), data.size());
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

		bool register_remote_addr(const InfoResponseLobby& lobby, netadr_t* addr)
		{
			const auto session_info{ get_session_info(lobby) };
			return register_remote_addr(&session_info, addr);
		}

		std::string build_lobby_msg(const game::LobbyModule module)
		{
			auto data{ "LM"s };
			data.push_back(0);
			const auto header{ 0x4864ui16 };
			data.append(reinterpret_cast<const char*>(&header), sizeof header);
			data.push_back(static_cast<std::uint8_t>(module));
			data.push_back(-1);

			return data;
		}
		
		void send_lobby_msg(const game::netadr_t& to, const msg_t& msg, const LobbyModule module)
		{
			auto data{ oob::build_lobby_msg(module) };
			data.append(reinterpret_cast<const char*>(msg.data), msg.cursize);
			send(to, data);
		}

		void initialize()
		{
			handlers["print"];
			handlers["echo"];
			handlers["statusResponse"];
			handlers["rcon"];
			handlers["RA"];

			for (const auto& handler : game::handlers)
			{
				events::connectionless_packet::on_command(handler.first, [=](const auto& args, const auto& target, const auto&)
				{
					return true;
				});
			}
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

		oob::initialize();
		rendering::initialize();
		exception::initialize();
		events::initialize();
		security::initialize();
		misc::initialize();
		command::initialize();
		steam::initialize();
		friends::initialize();

		PRINT_LOG("Initialized!");
	}

	bool in_game()
	{
		if (cg() == nullptr)
			return false;

		return LobbyClientLaunch_IsInGame() && cg()->clients[cg()->predictedPlayerState.clientNum].infoValid;
	}

	bool is_valid_target(const int client_num)
	{
		if (client_num >= 0 && client_num < 18)
			return session->clients[client_num].activeClient;

		return false;
	}

	void on_every_frame()
	{
		if (!game::in_game())
		{
			return;
		}
		
		aimbot::run(&cg()->predictedPlayerState);
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
	
	int find_target_from_addr(const netadr_t& from)
	{
		for (size_t i = 0; i < 18; ++i)
		{
			if (const auto client = &session->clients[i]; client && client->activeClient)
			{
				const auto session_info = client->activeClient->sessionInfo[session->type];
				if (game::NET_CompareAdr(from, session_info.netAdr))
					return i;
			}
		}

		return -1;
	}

	bool can_connect_to_player(const size_t client_num, const size_t target_xuid)
	{
		if (game::LiveUser_IsXUIDLocalPlayer(target_xuid))
			return true;

		const auto our_client_num = game::LobbySession_GetClientNumByXuid(game::LiveUser_GetXuid(0));
		if (our_client_num < 0)
			return false;

		return (1 << our_client_num) & session->clients[client_num].voiceInfo.connectivityBits;
	}

	game::XSESSION_INFO get_session_info(const game::InfoResponseLobby& lobby)
	{
		game::XSESSION_INFO sess_info{};
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
		const static auto CG_GetPlayerViewOrigin = reinterpret_cast<bool(*)(LocalClientNum_t, const playerState_s*, Vec3*, uint32_t)>(base_address + 0x11EF4C0); 
		return spoof_call::call(CG_GetPlayerViewOrigin, 0u, ps, view_origin, 0u);
	}

	void adjust_user_cmd_movement(usercmd_s* cmd_old, const float angle, const float old_angle)
	{
		const auto delta_view = DEG2RAD(angle - old_angle);

		cmd_old->forwardmove = ClampChar(static_cast<int>(std::cosf(delta_view) * cmd_old->forwardmove - std::sinf(delta_view) * cmd_old->rightmove));
		cmd_old->rightmove = ClampChar(static_cast<int>(std::sinf(delta_view) * cmd_old->forwardmove + std::cosf(delta_view) * cmd_old->rightmove));
	}

	float get_weapon_damage_range(const Weapon& weapon)
	{
		const auto weap_def = BG_GetWeaponDef(weapon); 
		
		if (weap_def->weapClass == WEAPCLASS_SPREAD || weap_def->weapClass == WEAPCLASS_PISTOL_SPREAD)
		{
			return BG_GetMinDamageRangeScaled(weapon) * BG_GetMultishotBaseMinDamageRangeScaled(weapon);
		}

		return sv_bullet_range->current.value;
	}

	bool CG_BulletTrace(game::BulletTraceResults* br, game::BulletFireParams* bp, const int attacker_entity_num, int lastSurfaceType)
	{
		const static auto CG_BulletTrace = reinterpret_cast<bool(*)(LocalClientNum_t, game::BulletFireParams *, game::BulletTraceResults*, const game::Weapon, int, int)>(game::base_address + 0x1168BA0);
		return CG_BulletTrace(0, bp, br, {}, attacker_entity_num, lastSurfaceType);
	}

	void send_instant_message(const std::vector<std::uint64_t>& recipients, std::uint8_t type, const void* message, const std::uint32_t message_size)
	{
		return dwInstantSendMessage(0, recipients.data(), recipients.size(), type, message, message_size);
	}

	bool send_netchan_message(const netadr_t& netadr, const std::uint64_t xuid, const std::string& data)
	{
		const auto channel = LobbyNetChan_GetLobbyChannel(session->type, LOBBY_CHANNEL_UNRELIABLE);
		return Netchan_SendMessage(0, channel, NETCHAN_UNRELIABLE, data.data(), data.size(), xuid, netadr, nullptr);
	}

	SessionClient* LobbySession_GetClientByXuid(const std::uint64_t xuid)
	{
		for (size_t i = 0; i < 18; ++i)
		{
			const auto client = &session->clients[i];

			if (client->xuid == xuid)
				return client;
		}

		return nullptr;
	}

	int LobbySession_GetClientNumByXuid(const std::uint64_t xuid)
	{
		for (size_t i = 0; i < 18; ++i)
		{
			const auto client = &session->clients[i];

			if (client->xuid == xuid)
				return i;
		}

		return -1;
	}

	bool is_valid_lobby_type(const int lobby_type)
	{
		return lobby_type >= LOBBY_TYPE_PRIVATE && lobby_type < LOBBY_TYPE_TRANSITION;
	}

	bool MSG_JoinMemberInfo(Msg_JoinMemberInfo* thisptr, game::msg_t* msg)
	{
		return game::LobbyMsgRW_PackageGlob(msg, "serializedadr", &thisptr->serializedAdr, sizeof thisptr->serializedAdr)
			&& game::LobbyMsgRW_PackageUInt64(msg, "reservationkey", &thisptr->reservationKey)
			&& game::LobbyMsgRW_PackageInt(msg, "lobbytype", &thisptr->targetLobby);
	}
	
	void enum_assets(const XAssetType type, const std::function<void(XAssetHeader)>& callback, const bool includeOverride)
	{
		DB_EnumXAssets(type, static_cast<void(*)(XAssetHeader, void*)>([](XAssetHeader header, void* data)
		{
			const auto& cb = *static_cast<const std::function<void(XAssetHeader)>*>(data);
			cb(header);
		}), &callback, includeOverride);
	}

	Material* get_material(const char* material_ptr, const int length)
	{
		if (length < 0 || length > 255)
			return nullptr;

		if (std::strlen(material_ptr) >= length)
			return Material_RegisterHandle(material_ptr, 7, true, -1);

		return nullptr;
	}

	bool is_invalid_material_pointer(const char* string)
	{
		const auto material = get_material(string + 4, string[3]);
		if (material && material->techniqueSet && material->techniqueSet->name == "2d_blend#da7372ff"s)
		{
			return false;
		}

		return true;
	}

	char CL_DeathMessageIconDimension(const float icon_width)
	{
		float v1; // ST00_4
		signed int v2; // eax

		v2 = (icon_width * 32.0f) + 9.313225746154785e-10;

		if (v2 >= 127)
			return 143;

		if (v2 <= 16)
			v2 = 16;

		return v2 + 16;
	}

	std::string CL_AddMessageIcon(const std::string& name, const Vec2& dimensions, const bool flip_icon)
	{
		auto string{ "^H"s };
		string += CL_DeathMessageIconDimension(dimensions.x);
		string += CL_DeathMessageIconDimension(dimensions.y);
		string += name.length();
		string += name;
		return string;
	}
}