#include "dependencies/std_include.hpp"
#include "lobby_msg.hpp"

namespace events::lobby_msg
{
	bool log_messages = true;

	namespace
	{
		bool handle_host_disconnect_client(const game::netadr_t& from, game::msg_t& msg)
		{
			PRINT_MESSAGE("LobbyMSG", "Disconnect prevented from %s", utils::string::adr_to_string(&from).data());
			return true;
		}

		bool handle_modified_stats(const game::netadr_t& from, game::msg_t& msg)
		{
			if (int stats_size{ 0 }; game::LobbyMsgRW_PackageInt(&msg, "statssize", &stats_size))
			{
				if (game::Com_SessionMode_IsMode(game::MODE_CAMPAIGN)
					&& game::Com_IsInGame()
					&& stats_size < 65536u)
				{
					return false;
				}

				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::string::adr_to_string(&from).data());
				return true;
			}

			return false;
		}

		bool handle_client_heartbeat(const game::netadr_t& from, game::msg_t& msg)
		{
			if (game::Msg_LobbyClientHeartbeat data{}; game::MSG_LobbyClientHeartbeat(&data, &msg))
			{
				if (const auto sess = game::get_host_session(data.lobbyType); sess) 
				{
					return false;
				}

				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::string::adr_to_string(&from).data());
				return true;
			}

			return false;
		}

		bool handle_client_content(const game::netadr_t& from, game::msg_t& msg)
		{
			if (game::Msg_ClientContent data{}; game::MSG_ClientContent(&data, &msg))
			{
				if (data.lobbyType >= game::LOBBY_TYPE_PRIVATE 
					&& data.lobbyType < game::LOBBY_TYPE_TRANSITION
					&& data.compressedBufferSize < sizeof data.clientContentData)
				{
					return false;
				}

				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::string::adr_to_string(&from).data());
				return true;
			}

			return false;
		}

		bool handle_join_request(const game::netadr_t& from, game::msg_t& msg)
		{
			if (game::Msg_JoinParty data{}; game::MSG_JoinParty(&data, &msg))
			{
				if (game::is_valid_lobby_type(data.targetLobby)
					&& game::is_valid_lobby_type(data.sourceLobby)
					&& data.memberCount < 18u)
				{
					return false;
				}

				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::string::adr_to_string(&from).data());
				return true;
			}
			
			return false;
		}

		bool handle_member_info(const game::netadr_t& from, game::msg_t& msg)
		{
			if (game::Msg_JoinMemberInfo data{}; game::MSG_JoinMemberInfo(&data, &msg))
			{
				if (game::is_valid_lobby_type(data.targetLobby))
				{
					return false;
				}

				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::string::adr_to_string(&from).data());
				return true;
			}

			return false;
		}

		bool handle_lobby_state(const game::netadr_t& from, game::msg_t& msg)
		{
			if (game::Msg_LobbyState data{}; game::MSG_LobbyState(&data, &msg))
			{
				if (game::is_valid_lobby_type(data.lobbyType)
					&& data.clientCount < 18u)
				{
					return false;
				}

				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::string::adr_to_string(&from).data());
				return true;
			}

			return false;
		}

		bool handle_lobby_type(const game::netadr_t& from, game::msg_t& msg)
		{
			if (int lobby_type_target{ -1 }; game::LobbyMsgRW_PackageInt(&msg, "lobbytype", &lobby_type_target))
			{
				if (game::is_valid_lobby_type(lobby_type_target))
				{
					return false;
				}
				
				PRINT_MESSAGE("LobbyMSG", "Crash attempt caught from %s", utils::string::adr_to_string(&from).data());
				return true;
			}

			return false;
		}

		bool handle_client_msg(const game::netadr_t& from, game::msg_t& msg)
		{
			if (msg.type == game::MESSAGE_TYPE_PEER_TO_PEER_INFO
				|| msg.type == game::MESSAGE_TYPE_LOBBY_CLIENT_DISCONNECT
				|| msg.type == game::MESSAGE_TYPE_VOICE_PACKET)
			{
				return handle_lobby_type(from, msg);
			}
			else if (msg.type == game::MESSAGE_TYPE_LOBBY_CLIENT_HEARTBEAT)
			{
				return handle_client_heartbeat(from, msg);
			}
			else if (msg.type == game::MESSAGE_TYPE_JOIN_LOBBY)
			{
				return handle_join_request(from, msg);
			}
			else if (msg.type == game::MESSAGE_TYPE_JOIN_MEMBER_INFO)
			{
				return handle_member_info(from, msg);
			}
			else if (msg.type == game::MESSAGE_TYPE_LOBBY_CLIENT_CONTENT)
			{
				return handle_client_content(from, msg);
			}
			else if (msg.type == game::MESSAGE_TYPE_LOBBY_MODIFIED_STATS)
			{
				return handle_modified_stats(from, msg);
			}

			return false;
		}

		bool handle_host_msg(const game::netadr_t& from, game::msg_t& msg)
		{
			if (msg.type == game::MESSAGE_TYPE_LOBBY_HOST_DISCONNECT
				|| msg.type == game::MESSAGE_TYPE_LOBBY_HOST_LEAVE_WITH_PARTY
				|| msg.type == game::MESSAGE_TYPE_LOBBY_MIGRATE_ANNOUNCE_HOST
				|| msg.type == game::MESSAGE_TYPE_INGAME_MIGRATE_TO
				|| msg.type == game::MESSAGE_TYPE_INGAME_MIGRATE_NEW_HOST)
			{
				return handle_lobby_type(from, msg);
			}
			else if (msg.type == game::MESSAGE_TYPE_LOBBY_HOST_DISCONNECT_CLIENT)
			{
				return handle_host_disconnect_client(from, msg);
			}

			return false;
		}

		bool handle_peer_to_peer_msg(const game::netadr_t& from, game::msg_t& msg)
		{
			return handle_lobby_type(from, msg);
		}
		
		std::unordered_map<game::LobbyModule, callback>& get_callbacks()
		{
			static std::unordered_map<game::LobbyModule, callback> callbacks{};
			return callbacks;
		}

		bool handle_packet(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			const auto& callbacks = get_callbacks();
			const auto handler = callbacks.find(module);

			if (handler == callbacks.end())
			{
				return false;
			}

			const auto msg_backup = msg;
			const auto callback = handler->second(from, msg);

			if (msg.readcount != msg_backup.readcount)
				msg = msg_backup;

			return callback;
		}

		bool __fastcall callback_handle_packet(const game::netadr_t& from, game::msg_t& msg, game::LobbyModule module)
		{
			const auto ip_str{ utils::string::adr_to_string(&from) };
			const auto type_name{ game::LobbyTypes_GetMsgTypeName(msg.type) };

			if (log_messages)
			{
				PRINT_LOG("Received lobby message [%i] <%s> from %s", module, type_name, ip_str.data());
			}
			
			return handle_packet(from, msg, module);
		}

		size_t handle_packet_stub()
		{
			const auto stub = utils::hook::assemble([](auto& a)
			{
				const auto return_original = a.newLabel();

				a.lea(r9, ptr(rsp, 0x30)); // message
				a.mov(ecx, ptr(rsp, 0xB0));

				a.pushad64();
				a.mov(r8, ecx); // lobby_module
				a.mov(rdx, r9); // message
				a.mov(rcx, rbp); // netadr
				a.call_aligned(lobby_msg::callback_handle_packet);
				a.test(al, al);
				a.jz(return_original);
				a.popad64();

				a.jmp(game::base_address + 0x1EF7181);

				a.bind(return_original);
				a.popad64();
				a.jmp(game::base_address + 0x1EF709B);
			});

			return reinterpret_cast<size_t>(stub);
		}
	}

	void on_message(const game::LobbyModule& module, const callback& callback)
	{
		get_callbacks()[module] = callback;
	}

	std::string build_lobby_msg(const game::LobbyModule module)
	{
		auto data{ ""s };
		const auto header = 0x4864ui16;
		data.append(reinterpret_cast<const char*>(&header), sizeof header);
		data.push_back(module);
		data.push_back(-1);
		return data;
	}

	bool send_to_host_unreliably(const game::msg_t& msg, const game::LobbyModule module)
	{
		auto data{ lobby_msg::build_lobby_msg(module) };
		data.append(reinterpret_cast<const char*>(msg.data), msg.cursize);
		return game::send_netchan_message(game::session->host.info.netAdr, game::session->host.info.xuid, data);
	}
	
	bool send_to_client_unreliably(const game::netadr_t& netadr, const std::uint64_t xuid, const game::msg_t& msg, const game::LobbyModule module)
	{
		auto data{ lobby_msg::build_lobby_msg(module) };
		data.append(reinterpret_cast<const char*>(msg.data), msg.cursize);
		return game::send_netchan_message(netadr, xuid, data);
	}

	void initialize()
	{
		exception::hwbp::register_hook(game::base_address + 0x1EF7094, events::lobby_msg::handle_packet_stub); 
		
		lobby_msg::on_message(game::LOBBY_MODULE_CLIENT, &handle_host_msg);
		lobby_msg::on_message(game::LOBBY_MODULE_HOST, &handle_client_msg);
		lobby_msg::on_message(game::LOBBY_MODULE_PEER_TO_PEER, &handle_peer_to_peer_msg);
	}
}