#include "dependencies/std_include.hpp"
#include "events.hpp"

namespace events
{
	bool prevent_join = true, no_presence = true, spoof_ip = true, block_p2p_packets = true;

	std::unordered_map<game::netadr_t, std::string> real_addr;

	namespace
	{
		utils::hook::detour cl_create_new_commands_hook;
		utils::hook::detour netchan_get_message_hook;
		utils::hook::detour process_packet_hook;
		utils::hook::detour sys_send_packet_hook;
		utils::hook::detour get_nat_type_hook;
		utils::hook::detour live_presence_pack_hook;
		
		void __fastcall cl_create_new_commands(LocalClientNum_t localClientNum)
		{
			cl_create_new_commands_hook.call(localClientNum);
			
			if (!game::in_game())
				return;

			if (!game::centity(game::cg()->clientNum)->is_alive())
				return;

			auto cmd_cur = game::cl()->user_cmd(game::cl()->cmdNumber);
			auto cmd_old = game::cl()->user_cmd(game::cl()->cmdNumber - 1);
			auto cmd_new = game::cl()->user_cmd(++game::cl()->cmdNumber);

			*cmd_new = *cmd_cur;
			cmd_cur->serverTime -= 1;
			cmd_old->serverTime += 1;

			static std::uint32_t angle_backup[3];
			VECTOR_COPY(cmd_old->angles, angle_backup);
			VECTOR_COPY(angle_backup, cmd_cur->angles);

			static std::uint32_t angle_backup_2[3];
			VECTOR_COPY(cmd_cur->angles, angle_backup_2);
			VECTOR_COPY(angle_backup_2, cmd_new->angles);

			const auto is_akimbo_gun = game::BG_IsDualWield(cmd_cur->weapon);
			const auto is_auto_fire = aimbot::aim_target && aimbot::auto_fire;
			auto button_flag = 0;

			if (is_auto_fire)
			{
				button_flag |= 0x80000000;

				if (is_akimbo_gun)
					button_flag |= 0x100080;

				cmd_cur->button_bits[0] |= button_flag;
			}

			auto cmd_angles = cmd_old->get_angles();

			if (aimbot::aim_target)
			{
				cmd_angles += aimbot::aim_angles;
				cmd_angles = math::angle_normalize_360(cmd_angles);
			}

			if (nospread::enabled)
			{
				const auto spread_angles = nospread::get_spread_angles(
					&game::cg()->predictedPlayerState,
					cmd_cur);

				cmd_angles += spread_angles;
				cmd_angles = math::angle_normalize_360(cmd_angles);
			}

			cmd_old->set_angles(cmd_angles);

			if (aimbot::aim_target && aimbot::silent)
			{
				game::adjust_user_cmd_movement(cmd_cur, cmd_old, SHORT2ANGLE(cmd_old->angles[1]));
			}
		}

		bool __fastcall live_presence_update()
		{
			const auto no_update = events::no_presence;
			auto presence_data = reinterpret_cast<game::PresenceData*>(OFFSET(0x7FF6D66EFBE0));

			if (!presence_data->isInitialzied)
				presence_data->init();

			if (no_update)
			{
				*presence_data = {};
			}

			return no_update;
		}

		bool __fastcall cl_ready_to_send_packet(LocalClientNum_t localClientNum)
		{
			if (game::net::netchan::writing)
			{
				return game::net::netchan::writing = false;
			}
			
			return game::call<bool>(0x7FF6C65DFF80, localClientNum);
		}

		bool __fastcall send_member_info(ControllerIndex_t controllerIndex, game::NetChanMsgType channel, game::LobbyModule module, game::netadr_t netadr, uint64_t xuid, game::Msg_JoinMemberInfo* msgData)
		{
			if (events::spoof_ip && !game::LiveUser_IsXUIDLocalPlayer(xuid))
			{
				const auto xnaddr = reinterpret_cast<game::XNADDR*>(&msgData->serializedAdr.xnaddr);
				xnaddr->inaddr = htonl(0x01010101);
			}

			return game::call<bool>(0x7FF6C71C49C0, controllerIndex, channel, module, netadr, xuid, msgData);
		}

		void __fastcall serialize(game::bdCommonAddr* thisptr, char* buffer)
		{
			game::call(0x7FF6C7BE1560, thisptr, buffer);

			if (events::spoof_ip && thisptr->m_isLoopback)
			{
				const auto xnaddr = reinterpret_cast<game::XNADDR*>(buffer);
				xnaddr->inaddr = htonl(0x01010101);
			}
		}
		
		bool __fastcall netchan_get_message(ControllerIndex_t localControllerIndex, game::NetChanMsgType type, game::msg_t *msg, uint64_t *xuid, game::netadr_t *addr, uint32_t *sequence)
		{
			if (!netchan_get_message_hook.call<bool>(localControllerIndex, type, msg, xuid, addr, sequence))
				return false;

			if (const auto msg_backup = *msg; !game::net::netchan::get(game::s_netchan->in[type], msg, type))
			{
				*msg = msg_backup; 
				return true;
			}

			return false;
		}

		int __fastcall process_packet(void* thisptr, game::bdAddrHandleRef* addr_handle, game::bdAddr* addr, const uint8_t* data, const uint32_t size, const uint32_t bytes_transferred, bool* receiving)
		{
			const auto error = process_packet_hook.call<int>(thisptr, addr_handle, addr, data, size, bytes_transferred, receiving);

			if (error > 0 && addr_handle && addr_handle->ptr)
			{
				const auto netadr = addr_handle->ptr->get_addr();
				const auto xnaddr = netadr.to_xnaddr();

				if (*addr != xnaddr)
				{
					events::real_addr[netadr] = addr->to_string();
				}
			}

			return error;
		}
		
		bool __fastcall sys_send_packet(game::netsrc_t sock, int length, const uint8_t* data, game::netadr_t netadr)
		{
			if (const auto session = game::session_data())
			{
				const auto host_netadr = session->host.info.netAdr;
				
				const auto block = events::block_p2p_packets
					&& netadr.connected() && !netadr.local()
					&& host_netadr.connected() && !host_netadr.local()
					&& !game::NET_CompareAdr(netadr, host_netadr);

				if (block)
				{
					PRINT_LOG("Not sending P2P packet to %s", utils::get_sender_string(netadr).data());
					return false;
				}
			}
			
			return sys_send_packet_hook.call<bool>(sock, length, data, netadr);
		}
		
		int __fastcall live_presence_pack(game::PresenceData* presence, void* buffer, size_t buffer_size)
		{
			presence->title.party.max = std::numeric_limits<int>::max(); 
			presence->title.party.totalCount = std::numeric_limits<int>::max();
			presence->title.party.availableCount = std::numeric_limits<int>::max();

			for (size_t i = 0; i < 18; ++i)
			{
				const auto gamertag = "&&" + std::to_string(i);
				game::I_strncpyz(presence->title.party.members[i].gamertag, gamertag, sizeof(presence->title.party.members[i].gamertag));
			}

			return live_presence_pack_hook.call<int>(presence, buffer, buffer_size);
		}

		bool __fastcall cl_add_reliable_command(LocalClientNum_t localClientNum, const char* cmd)
		{
			if (session::simulate_join::enabled && session::simulate_join::ready)
			{
				const auto args = utils::string::split(cmd, ' ');

				if (args.size() <= 2)
					return false;

				auto chat_message = utils::string::join(args, 2);
				chat_message.resize(chat_message.size() - 2);
				
				return steam::send_lobby_chat_message(game::session_data()->lobbyData.platformSessionID, 0, chat_message);
			}

			return game::call<bool>(0x7FF6C663D4C0, localClientNum, cmd);
		}
		
		game::bdNATType __fastcall get_nat_type(const game::bdCommonAddr* thisptr)
		{
			if (thisptr->m_isLoopback)
			{
				return game::BD_NAT_OPEN;
			}

			return get_nat_type_hook.call<game::bdNATType>(thisptr);
		}
	}
	
	void initialize()
	{
		const auto cg_predict_playerstate_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.pushad64();
			a.call_aligned(events::cl_create_new_commands);
			a.popad64();
			
			a.jmp(OFFSET(0x7FF6C5CA3AF0));
		});

		const auto live_presence_update_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			const auto return_unhandled = a.newLabel();

			a.pushad64();
			a.call_aligned(events::live_presence_update);
			a.test(al, al);
			a.jz(return_unhandled);

			a.popad64();
			a.mov(al, 1);
			a.ret();

			a.bind(return_unhandled);
			a.popad64();
			a.jmp(OFFSET(0x7FF6C7174490));
		});

		sys_send_packet_hook.create(OFFSET(0x7FF6C7612F70), sys_send_packet);
		process_packet_hook.create(OFFSET(0x7FF6C7C3CE00), process_packet); 
		netchan_get_message_hook.create(OFFSET(0x7FF6C74553D0), netchan_get_message);
		cl_create_new_commands_hook.create(OFFSET(0x7FF6C65D6F50), cl_create_new_commands);
		get_nat_type_hook.create(OFFSET(0x7FF6C7BE1530), get_nat_type);
		//live_presence_pack_hook.create(OFFSET(0x7FF6C7173D70), live_presence_pack);
			
		utils::hook::call(OFFSET(0x7FF6C65E00A8), cl_ready_to_send_packet);
		utils::hook::call(OFFSET(0x7FF6C71C501C), send_member_info);
		utils::hook::call(OFFSET(0x7FF6C7C4AB87), serialize);
		//utils::hook::call(OFFSET(0x7FF6C639B99D), cg_predict_playerstate_stub);
		utils::hook::call(OFFSET(0x7FF6C7174708), live_presence_update_stub);
		utils::hook::call(OFFSET(0x7FF6C70CACB9), cl_add_reliable_command);

		utils::hook::set<int>(OFFSET(0x7FF6C82E154C), 0xD3FC12);
		
		connectionless_packet::initialize();
		instant_message::initialize();
		lobby_msg::initialize();
		server_command::initialize();
	}
}