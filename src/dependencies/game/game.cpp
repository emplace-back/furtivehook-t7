﻿#include "dependencies/std_include.hpp"
#include "game.hpp"

namespace game
{
	std::array<scr_string_t, static_cast<std::uint32_t>(bone_tag::num_tags)> bone_tags; 

	namespace net
	{
		namespace netchan
		{
			bool writing{ false }; 
			
			bool get(const NetChanMessage_s* chan, msg_t* msg, NetChanMsgType type)
			{
				if (!chan || chan->complete)
					return false;

				if (static_cast<size_t>(msg->maxsize) < chan->messageLen)
					return false;

				if (type == NETCHAN_VOICE)
				{
					const auto args = command::args::get_client(); 
					
					char buffer[1024] = { 0 };
					args.tokenize(game::call<const char*>(0x7FF6C7437BA0, msg, buffer, sizeof buffer));
					const auto _ = utils::finally([=]()
					{
						args.end_tokenize();
					});

					PRINT_LOG("Ignoring voice packet '%s' from %s", args.join(0).data(), utils::get_sender_string(chan->destAddress).data());
					return true;
				}

				return false;
			}

			bool send2(const NetChanMsgType type, const std::string& data, const netadr_t& netadr, const uint64_t target_xuid, const uint64_t sender_xuid)
			{
				const auto chan = reinterpret_cast<NetChanMessageList_s*>(OFFSET(0x7FF6DC14BE20));
				auto msg = chan->out[type];
				for (; msg; msg = msg->next)
				{
					if (msg->destXUID == target_xuid)
						break;
				}

				constexpr auto fragment_offset{ 1222 };
				const auto max_fragments{ static_cast<uint8_t>(data.size() / fragment_offset) };
				const auto fragment_length{ data.size() % fragment_offset };

				const auto sender_id{ sender_xuid ? sender_xuid : game::LiveUser_GetXuid(0) };

				if (type == NETCHAN_CLIENTMSG)
					netchan::writing = true;

				if (msg)
				{
					if (!msg->complete)
					{
						++msg->sequence;
					}

					//if (msg->dropped)
					//{
					//	++msg->nonce;
					//	msg->dropped = 0;
					//}

					msg->dropped = 0;
					msg->sendCount = 0;
					msg->messageLen = data.size();
					msg->numFragments = max_fragments;
					msg->complete = 0;
				}
				else
				{
					msg = game::call<NetChanMessage_s*>(0x7FF6C74543F0);

					if (!msg)
						return false;

					msg->destXUID = target_xuid;
					msg->sourceXUID = sender_id;
					msg->messageLen = data.size();
					msg->numFragments = max_fragments;
					msg->next = chan->out[type];
					msg->sendCount = 0;
					msg->sequence = 1;
					//msg->nonce = 1;
					chan->out[type] = msg;
				}

				const auto netchan_ms = *reinterpret_cast<int*>(OFFSET(0x7FF6DC12BB1C));
				msg->lastKeepAliveMs = netchan_ms;
				msg->lastTouchedMS = netchan_ms;
				msg->destAddress = netadr;
				msg->nonce = std::numeric_limits<uint16_t>::max();

				auto final_msg{ *msg };
				final_msg.sourceXUID = sender_id;

				for (size_t i = 0; i < max_fragments; ++i)
					game::call(0x7FF6C7454300, &data[i * fragment_offset], fragment_offset, i, &final_msg);

				if (fragment_length != 0)
				{
					game::call(0x7FF6C7454300, data.data(), fragment_length, max_fragments, &final_msg);
					++final_msg.numFragments;
				}

				return game::call<bool>(0x7FF6C74570F0, 0, type, &final_msg);
			}

			std::unordered_map<uint64_t, uint16_t> nonce_map;
			
			bool send(const NetChanMsgType type, std::string data, const netadr_t& netadr, uint64_t dest_xuid, uint64_t src_xuid, uint16_t nonce)
			{
				if (!dest_xuid)
				{
					dest_xuid = 0xDEADFA11;
				}

				if (!src_xuid)
				{
					src_xuid = 0x1111111100000000 + utils::random<uint32_t>();
				}
				
				bool result;
				
				constexpr auto max_fragment_size = 1222;

				if (data.size() > max_fragment_size * 128)
				{
					DEBUG_LOG("Netchan message too large to send (%llx)", data.size());
					return false;
				}

				while (data.size() % max_fragment_size)
				{
					data.resize(data.size() + 1);
				}

				auto data_length = data.size(); 
				
				auto num_fragments = std::max(1LLU, data_length / max_fragment_size);

				auto fragments = std::unique_ptr<game::NetChanFragment_s[]>(new game::NetChanFragment_s[num_fragments]);

				game::NetChanMessage_s msg{};

				msg.sequence = 1;

				if (nonce)
				{
					const auto chan = reinterpret_cast<game::NetChanMessageList_s*>(OFFSET(0x7FF6DC14BE20));
					const auto msg2 = chan->out[type]; 

					if (msg2)
						msg.sequence = msg2->sequence + 1;
				}

				msg.numFragments = static_cast<uint8_t>(num_fragments);
				msg.complete = false;
				msg.messageLen = static_cast<uint32_t>(data_length);
				msg.destXUID = dest_xuid;
				msg.sourceXUID = src_xuid;
				msg.sendCount = 0;
				msg.destAddress = netadr;
				msg.msgConfig = nullptr;
				msg.next = nullptr;
				msg.fragments = fragments.get();
				ZeroMemory(msg.acked, sizeof msg.acked);
				msg.lastAckMS = 0;
				msg.lastTouchedMS = 0;
				msg.lastKeepAliveMs = 0;
				msg.timeoutMS = 0;
				msg.dropped = false;
				msg.nonce = std::numeric_limits<uint16_t>::max();

				for (size_t i = 0; i < num_fragments; ++i)
				{
					auto& f = fragments[i];

					f.sendCount = 0;
					f.sequence = static_cast<uint8_t>(i);
					f.size = max_fragment_size;

					std::memcpy(
						f.msgBuf, 
						&data[i * max_fragment_size], 
						f.size);

					char buffer[1256] = { 0 };
					game::msg_t m{};

					m.init(buffer, sizeof(buffer));
					m.write<uint8_t>(0xE0);
					m.write<uint64_t>(msg.destXUID);
					m.write<uint64_t>(msg.sourceXUID);
					m.write<uint8_t>(static_cast<uint8_t>(type));
					m.write<uint16_t>(msg.nonce);
					m.write<uint32_t>(msg.sequence);
					m.write<uint8_t>(f.sequence);
					m.write<uint8_t>(msg.numFragments);
					m.write<uint16_t>(static_cast<uint16_t>(f.size));
					m.write_data(f.msgBuf, f.size);

					result = net::send(msg.destAddress, { m.data, static_cast<size_t>(m.cursize) });
				}

				return result;
			}

			bool send_oob(const netadr_t& netadr, const std::string& data)
			{
				auto final_data{ "\xff\xff\xff\xff" + data };
				return netchan::send(NETCHAN_CLIENTMSG, final_data, netadr);
			}

			bool write(const write_packet& packet, const netadr_t& netadr, const uint64_t sender_xuid, const uint16_t nonce)
			{
				char buffer[2048] = { 0 };
				msg_t msg{};

				msg.init(buffer, sizeof buffer);
				msg.write<uint8_t>(packet.server_id);
				msg.write<int>(packet.message_sequence);
				msg.write<int>(packet.command_sequence);

				if (!packet.data.empty())
				{
					msg.write_bits(2, 3);
					msg.write<int>(packet.acknowledge);
					msg.write_data(packet.data);
				}

				msg.write_bits(3, 3);

				constexpr auto COMPRESSED_OFFSET{ sizeof(uint64_t) + 1 };
				
				char compressed_buffer[2048] = { 0 };
				std::memcpy(compressed_buffer, msg.data, COMPRESSED_OFFSET);

				const auto compressed_size = game::call<size_t>(0x7FF6C74406C0, 
					false, 
					&msg.data[COMPRESSED_OFFSET], 
					msg.cursize - COMPRESSED_OFFSET, 
					&compressed_buffer[COMPRESSED_OFFSET], 
					msg.maxsize - COMPRESSED_OFFSET
				) + COMPRESSED_OFFSET;
				
				return netchan::send(NETCHAN_CLIENTMSG, { compressed_buffer, compressed_size }, netadr, 0, sender_xuid, nonce);
			}

			bool write(const std::string& data)
			{
				const auto session = game::session_data(); 
				const auto clc = game::clc();

				const write_packet packet =
				{
					game::cl()->serverId,
					clc->serverMessageSequence,
					clc->serverCommandSequence,
					clc->reliableCommands.acknowledge + 1,
					data,
				};

				return netchan::write(packet, session->host.info.netAdr, session->host.info.xuid);
			}
		}
		
		namespace oob
		{
			bool send(const netadr_t& netadr, const std::string& data)
			{
				const auto final_data{ "\xff\xff\xff\xff" + data };
				return net::send(netadr, final_data);
			}

			bool register_remote_addr(const HostInfo& info, netadr_t* addr)
			{
				dwRegisterSecIDAndKey(&info.secId, &info.secKey);

				if (!dwCommonAddrToNetadr(addr, &info.serializedAdr.xnaddr, &info.secId))
				{
					DEBUG_LOG("Failed to retrieve the remote IP address from XNADDR");
					return false;
				}

				return true;
			}

			netadr_t register_remote_addr(const InfoResponseLobby& lobby)
			{
				game::netadr_t netadr{};
				
				if (oob::register_remote_addr(game::HostInfo{}.from_lobby(lobby), &netadr))
				{
					return netadr;
				}
				
				return {};
			}

			void on_registered_addr(const HostInfo& info, const std::function<void(const game::netadr_t&)>& callback)
			{
				scheduler::schedule([=]()
				{
					game::netadr_t netadr{};
					game::net::oob::register_remote_addr(info, &netadr);

					const auto status = game::dwGetConnectionStatus(&netadr);

					switch (status)
					{
					case game::BD_SOCKET_CONNECTED:
						callback(netadr);
						return scheduler::cond_end;
					case game::BD_SOCKET_LOST:
						return scheduler::cond_end;
					default:
						return scheduler::cond_continue;
					}
				}, scheduler::main, 75ms);
			}

			void on_registered_addr(const InfoResponseLobby& lobby, const std::function<void(const game::netadr_t&)>& callback)
			{
				return oob::on_registered_addr(game::HostInfo{}.from_lobby(lobby), callback);
			}
		}

		bool send(const netadr_t& netadr, const std::string& data)
		{
			const auto network_id = game::call<int>(0x7FF6C73D03B0, 0);

			if (netadr.type != game::NA_LOOPBACK)
			{
				return game::call<bool>(0x7FF6C7612F70, network_id, data.size(), data.data(), &netadr);
			}

			return game::call<bool>(0x7FF6C7458600, 
				reinterpret_cast<void*>(OFFSET(0x7FF6DC14BC10) + 104i64 * netadr.port), 
				0, 
				network_id, 
				&netadr, 
				data.size(), 
				data.data());
		}
	}
	
	uintptr_t get_base()
	{
		static auto base{ utils::nt::library{}.get_ptr() };
		return reinterpret_cast<size_t>(base);
	}

	BOOL __stdcall enum_windows_proc(const HWND window, const LPARAM param)
	{
		DWORD process_id = 0;
		GetWindowThreadProcessId(window, &process_id);

		if (process_id == GetCurrentProcessId())
		{
			char class_name[255] = { 0 };
			GetWindowTextA(window, class_name, sizeof(class_name));

			if (class_name == "NI"s)
			{
				*reinterpret_cast<HWND*>(param) = window;
				return FALSE;
			}
		}

		return TRUE;
	}
	
	HWND get_game_window()
	{
		static HWND window = nullptr;
		if (!window || !IsWindow(window))
		{
			EnumWindows(enum_windows_proc, reinterpret_cast<LPARAM>(&window));
		}

		return window;
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

		command::initialize();
		session::initialize();
		steam::initialize();
		rendering::initialize();
		events::initialize();
		friends::initialize();

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
				if (NET_CompareBaseAdr(from, netadr))
					return i;
			}
		}

		return -1;
	}

	bool CG_WorldPosToScreenPos(const Vec3* pos, Vec2* out)
	{
		return call<bool>(0x7FF6C5854140, 0, pos, out);
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
		
		return call<bool>(0x7FF6C64D04C0, 0, ps, view_origin, 0);
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

		if (weapon_def->weapClass == WEAPCLASS_SPREAD || weapon_def->weapClass == WEAPCLASS_PISTOL_SPREAD)
		{
			return BG_GetMinDamageRangeScaled(weapon) * BG_GetMultishotBaseMinDamageRangeScaled(weapon);
		}

		return 8192.0f;
	}

	bool CG_BulletTrace(BulletTraceResults* br, BulletFireParams* bp, const int attacker_entity_num, int lastSurfaceType)
	{
		return call<bool>(0x7FF6C6449BA0, 0, bp, br, 0, attacker_entity_num, lastSurfaceType);
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

	const char* LobbyTypes_GetMsgTypeName(const MsgType type)
	{
		if (type < MESSAGE_TYPE_INFO_REQUEST || type > MESSAGE_TYPE_DEMO_STATE)
		{
			return "Invalid";
		}

		return lobbyMsgName[type];
	}

	bool LobbySession_IsDedicated(const game::LobbySession* session)
	{
		return game::LobbySession_GetClientNumByXuid(session, session->host.info.xuid) == -1;
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
			const auto result = game::call<bool>(0x7FF6C71D6520, msg);
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

	char* I_strncpyz(char* place, const std::string& string, const size_t length)
	{
		const auto result = std::strncpy(place, string.data(), length);
		place[length - 1] = 0;
		return result;
	}

	TaskRecord* TaskManager2_SetupRemoteTask(const TaskDefinition* definition, bdRemoteTask* remote_task, const uint32_t timeout)
	{
		if (!remote_task)
			return nullptr;
		
		const auto task = TaskManager2_CreateTask(definition, 0, nullptr, timeout);

		if (!task)
			return nullptr;

		task->remoteTask = remote_task;
		TaskManager2_StartTask(task);

		return task;
	}
	
	bool connect_to_session(const game::HostInfo& info)
	{
		const auto begin = game::call<bool>(0x7FF6C71B9540, 0, 0, game::LOBBY_TYPE_PRIVATE, game::LOBBY_TYPE_GAME);

		if (!begin)
			return false;

		game::call(0x7FF6C71C4550, info.xuid, info.name, &info.secId, &info.secKey, &info.serializedAdr, game::JOIN_TYPE_NORMAL, info.xuid);
		game::call(0x7FF6C71C5DD0);
		return true;
	}

	bool NET_CompareXNAddr(const XNADDR* a, const XNADDR* b)
	{
		return std::memcmp(a, b, sizeof game::XNADDR) == 0;
	}

	netadr_t get_netadr_from_xuid(const LobbySession* session, const uint64_t xuid)
	{
		if (session == nullptr)
			return {};

		const auto client_num = LobbySession_GetClientNumByXuid(session, xuid);

		if (client_num == -1)
			return {};

		const auto client = session->clients[client_num].activeClient;

		if (!client)
			return {};

		return get_session_netadr(session, client);
	}
}