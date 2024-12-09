#include "dependencies/std_include.hpp"
#include "session.hpp"

namespace session
{
	namespace simulate_join
	{
		bool enabled{ true }; 
		bool ready{ false };

		namespace
		{
			bool send_simulated_join()
			{
				if (!simulate_join::enabled)
					return false;

				if (!simulate_join::ready)
					return false;

				const auto clc = game::clc();

				if (!clc || !clc->serverAddress.connected() || clc->serverAddress.local())
				{
					simulate_join::ready = false;
					return false;
				}

				char buffer[0x800] = { 0 };
				game::msg_t msg{};

				msg.init_lobby(buffer, game::MESSAGE_TYPE_JOIN_LOBBY);

				const auto xuid = utils::random<uint64_t>();

				game::Msg_JoinParty join_party{};
				join_party.targetLobby = game::LOBBY_TYPE_GAME;
				join_party.memberCount = 1;
				join_party.members[0].xuid = xuid;
				join_party.members[0].lobbyID = xuid;
				join_party.playlistID = std::numeric_limits<uint8_t>::max();
				join_party.playlistVersion = game::call<int>(0x7FF6C7574530); // Playlist_GetVersionNumber
				join_party.ffotdVersion = game::call<int>(0x7FF6C71A41A0); // LiveStorage_GetFFOTDVersion
				join_party.networkMode = game::call<game::LobbyNetworkMode>(0x7FF6C71C81B0); // LobbyBase_GetNetworkMode
				join_party.netFieldChecksum = game::call<int>(0x7FF6C74398B0); // MsgCRC_NetFieldChecksum
				join_party.changelist = game::call<int>(0x7FF6C73D0030); // Com_GetChangelist
				join_party.protocolVersion = events::lobby_msg::PROTOCOL;
				join_party.dlcBits = std::numeric_limits<uint32_t>::max();
				join_party.joinNonce = xuid;
				for (size_t i = 0; i < sizeof(join_party.chunkStatus); ++i) join_party.chunkStatus[i] = game::call<uint8_t>(0x7FF6C7434DE0, i); // Install_IsChunkInstalled
				game::call(0x7FF6C71C2110, &join_party, &msg); // Msg_JoinParty::Package

				return events::lobby_msg::send_lobby_msg(
					game::call<game::NetChanMsgType>(0x7FF6C71D8F40, 0), 
					game::LOBBY_MODULE_HOST, 
					msg, 
					clc->serverAddress);
			}

			bool send_disconnect(const game::netadr_t& from)
			{
				if (!simulate_join::enabled)
					return false;

				if (simulate_join::ready)
					return false;

				if (!game::LobbySession_IsDedicated(game::session_data()))
					return false;

				if (!game::NET_CompareAdr(from, game::clc()->serverAddress))
					return false;

				const auto our_xuid = game::LiveUser_GetXuid(0);

				for (size_t i = 0; i < sizeof(uint32_t); ++i)
				{
					simulate_join::ready |= exploit::lobby_msg::send_client_disconnect(from, game::LOBBY_TYPE_GAME, our_xuid);
				}

				return simulate_join::ready;
			}
		}

		void initialize()
		{
			scheduler::loop(simulate_join::send_simulated_join, scheduler::main, 750ms);

			events::lobby_msg::on_message(game::LOBBY_MODULE_HOST, game::MESSAGE_TYPE_JOIN_RESPONSE, [=](const game::netadr_t& from, game::msg_t& msg, game::LobbyModule)
			{
				if (!simulate_join::enabled)
					return false; 

				if (!simulate_join::ready)
					return false;
					
				if (!game::NET_CompareAdr(from, game::clc()->serverAddress))
					return false;

				game::Msg_JoinResponse join_response{};

				if (!game::call<bool>(0x7FF6C71C2470, &join_response, &msg)) // Msg_JoinMemberInfo::Package
					return false;

				if (join_response.joinResult == game::JOIN_RESULT_SUCCESS)
				{
					game::Msg_JoinMemberInfo member_info{};
					auto& fixed_info = member_info.fixedClientInfoMsg.fixedClientInfo;
					game::call(0x7FF6C71AB620, 0, &member_info.mutableClientInfoMsg.mutableClientInfo); // LobbyClient_GetMutableClientInfo
					game::call(0x7FF6C71AB100, 0, utils::random<uint16_t>(), &fixed_info); // LobbyClient_GetFixedClientInfo

					const auto xuid = utils::random<uint64_t>();
					fixed_info.xuid = xuid;
					game::I_strncpyz(fixed_info.gamertag, utils::string::random(16), sizeof(fixed_info.gamertag));
					
					member_info.reservationKey = join_response.reservationKey;
					member_info.targetLobby = join_response.lobbyType;

					game::bdCommonAddr* addr{ nullptr };
					addr->get_local_addr(&member_info.serializedAdr.xnaddr);

					char buffer[0x800] = { 0 };
					game::msg_t msg{};

					msg.init_lobby(buffer, game::MESSAGE_TYPE_JOIN_MEMBER_INFO);
					game::LobbyMsgRW_PackageGlob(&msg, "serializedadr", &member_info.serializedAdr.xnaddr, sizeof(member_info.serializedAdr.xnaddr));
					game::LobbyMsgRW_PackageUInt64(&msg, "reservationkey", reinterpret_cast<uint64_t*>(&member_info.reservationKey));
					game::LobbyMsgRW_PackageInt(&msg, "lobbytype", reinterpret_cast<int*>(&member_info.targetLobby));
					game::call(0x7FF6C71C5210, &member_info.fixedClientInfoMsg, &msg); // MsgFixedClientInfo::Package
					game::call(0x7FF6C71B4860, &member_info.mutableClientInfoMsg, &msg); // MsgMutableClientInfo::Package

					events::lobby_msg::send_lobby_msg(
						game::call<game::NetChanMsgType>(0x7FF6C71D8F40, 0), 
						game::LOBBY_MODULE_HOST,
						msg, 
						from);
				} 

				return true;
			});

			events::connectionless_packet::on_command("steamAuthReq", [=](const auto&, const auto& from, const auto&)
			{
				simulate_join::send_disconnect(from);
				return false;
			});
		}
	}
	
	namespace
	{
		struct player_info
		{
			size_t client_num; 
			std::string name;
		}; 
		
		game::MatchMakingQuery query_info{};
		std::array<game::MatchMakingInfo, 500> search_results;
		std::vector<game::MatchMakingInfo> sessions;

		std::mutex search_mutex;
		
		std::unordered_map<game::netadr_t, std::vector<player_info>> players;
		
		using pair = std::pair<std::string, std::string>;
		std::unordered_map<game::netadr_t, pair> info;

		const auto lobby_search_success_callback = [](game::TaskRecord* task)
		{
			const auto remote_task = task->remoteTask;

			if (!remote_task)
				return;

			for (size_t i = 0; i < remote_task->numResults; ++i)
			{
				const auto r = &search_results[i];

				if (r->info.hostAddrSize != sizeof game::XNADDR)
					continue;

				session::register_session(*r);
			}
		};
			
		const auto lobby_search_failure_callback = [](game::TaskRecord*)
		{
			PRINT_LOG("Session search task failed.");
		};
			
		const game::TaskDefinition task_lobby_search
		{
			2,
			"t71",
			0x10,
			lobby_search_success_callback,
			lobby_search_failure_callback
		};

		void fetch_sessions()
		{
			const std::lock_guard<std::mutex> _(search_mutex);
			
			if (game::TaskManager2_TaskGetInProgress(&task_lobby_search))
				return;

			game::TaskManager2_ClearTasks(&task_lobby_search);

			const auto task = game::TaskManager2_CreateTask(&task_lobby_search, 0, nullptr, 10000);

			if (!task)
				return;

			// dwGetMatchmaking
			const auto matchmaking = game::call<uintptr_t>(0x7FF6C672B2D0, 0);

			if (!matchmaking)
				return;

			query_info.queryId = game::SEARCH_SESSIONS;
			query_info.serverType = std::numeric_limits<int>::max();

			game::bdRemoteTask* remote_task{ nullptr };
			
			// bdMatchMaking::findSessions
			game::call<game::bdRemoteTask*>(0x7FF6C7BEB2B0,
				matchmaking,
				&remote_task,
				query_info.queryId,
				0,
				search_results.size(),
				&query_info,
				search_results.data());

			if (!remote_task)
			{
				game::TaskManager2_RevertTask(task);
				return;
			}

			task->remoteTask = remote_task;
			game::TaskManager2_StartTask(task);
		}
	}
	
	game::MatchMakingInfo* get(const uint64_t xuid)
	{
		const auto entry = std::find_if(sessions.begin(), sessions.end(), [&](const auto& s) { return s.xuid == xuid; });

		if (entry != sessions.end())
		{
			return &*entry;
		}

		return nullptr;
	}
	
	void register_session(const game::MatchMakingInfo& session)
	{
		if (const auto s = session::get(session.xuid))
		{
			*s = session;
		}
		else
		{
			sessions.emplace_back(session);
		}
	}
	
	void draw_session_list(const float width, const float spacing)
	{
		if (ImGui::BeginTabItem("Sessions"))
		{
			const auto session_data = game::session_data();
			
			static ImGuiTextFilter filter;
			ImGui::TextUnformatted("Search session");
			filter.Draw("##search_session", width * 0.85f);

			ImGui::Separator();

			ImGui::BeginColumns("Sessions", 2, ImGuiColumnsFlags_NoResize);

			ImGui::TextUnformatted("Session");
			ImGui::NextColumn();
			ImGui::SetColumnOffset(-1, width - ImGui::CalcTextSize("Clients").x);
			ImGui::TextUnformatted("Clients");
			ImGui::NextColumn();

			ImGui::Separator();

			std::vector<game::MatchMakingInfo> sorted_sessions{ sessions };
			std::sort(sorted_sessions.begin(), sorted_sessions.end());

			for (const auto& session : sorted_sessions)
			{
				const auto host_info = session.get_host_info();
				const auto xnaddr = host_info.serializedAdr.xnaddr;
				const auto ip_string = xnaddr.to_string(true);

				if (!filter.PassFilter(ip_string))
					continue;

				const auto xuid = std::to_string(static_cast<int64_t>(session.xuid));
				const auto label = ip_string + "##session" + xuid;
				const auto selected = ImGui::Selectable(label);
				const auto popup = "session_popup##" + xuid;

				ImGui::AlignTextToFramePadding();

				if (session_data && session_data->host.info.xuid == host_info.xuid)
				{
					ImGui::SameLine(0, spacing);
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 0.95f), "[Current]");
				}

				if (selected)
					ImGui::OpenPopup(popup.data());

				if (ImGui::BeginPopup(popup.data(), ImGuiWindowFlags_NoMove))
				{
					if (ImGui::MenuItem(ip_string))
					{
						ImGui::LogToClipboardUnformatted(ip_string);
					}

					if (ImGui::MenuItem(xuid))
					{
						ImGui::LogToClipboardUnformatted(xuid);
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Join Session"))
					{
						game::connect_to_session(host_info);
					}

					ImGui::Separator();

					if (ImGui::BeginMenu("Send OOB##" + xuid))
					{
						static auto oob_input = ""s;

						ImGui::InputTextWithHint("##" + xuid, "OOB", &oob_input);

						if (ImGui::MenuItem("Send OOB", nullptr, nullptr, !oob_input.empty()))
						{
							game::net::oob::on_registered_addr(host_info, [=](const auto& netadr)
								{
									game::net::netchan::send_oob(netadr, oob_input);
								});
						}

						ImGui::EndMenu();
					}

					ImGui::Separator();

					if (ImGui::BeginMenu("List players##" + xuid))
					{
						game::netadr_t netadr{};
						game::net::oob::register_remote_addr(session.get_host_info(), &netadr);

						if (ImGui::MenuItem("Refresh players"))
						{
							game::net::netchan::send_oob(netadr, "getstatus\n");
						}

						ImGui::Separator();

						const auto& info = session::info[netadr];
						const auto& mapname = info.first;
						const auto& gametype = info.second;

						if (!mapname.empty() && !gametype.empty())
						{
							char buffer[128] = { 0 };
							game::call(0x7FF6C73D3F40, gametype.data(), mapname.data(), buffer, sizeof buffer); // Com_GameInfo_GetGameTypeOnMapName

							ImGui::MenuItem(buffer);
						}

						ImGui::Separator();

						for (const auto& player : players[netadr])
						{
							const auto client_num = player.client_num;
							const auto label = player.name + "##player";
							const auto selected = ImGui::Selectable(label);
						}

						ImGui::EndMenu();
					}

					ImGui::EndPopup();
				}

				ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();

				ImGui::Text("%i / %i", session.info.numPlayers, session.info.maxPlayers);

				ImGui::NextColumn();

				if (ImGui::GetColumnIndex() == 0)
				{
					ImGui::Separator();
				}
			}

			ImGui::EndColumns();
			ImGui::EndTabItem();
		}
	}
	
	void initialize()
	{
		simulate_join::initialize();
		
		// Setup query info
		game::call(0x7FF6C672D2E0, &query_info, 0, true);

		// Setup search results
		game::call(0x7FF6C7F2486C, &search_results, sizeof(game::MatchMakingInfo), search_results.size(), OFFSET(0x7FF6C672CA00), OFFSET(0x7FF6C6718960));

		scheduler::on_dw_initialized([]()
		{
			scheduler::once(session::fetch_sessions, scheduler::pipeline::main);

			scheduler::loop([]()
			{
				if (menu::is_open())
				{
					session::fetch_sessions();
				}
			}, scheduler::pipeline::main, 500ms);
		}, scheduler::pipeline::main);
		
		events::connectionless_packet::on_command("statusResponse", [](const auto& args, const auto& from, auto& msg)
		{
			char buffer[1024]{ 0 };
			game::call(0x7FF6C7437BA0, &msg, buffer, sizeof buffer);

			utils::string::info_string info{ buffer };
			
			session::info[from] = 
			{ 
				info.get("mapname") , 
				info.get("g_gametype"),
			};

			std::vector<player_info> players_info;
			players_info.reserve(18);

			for (size_t i = 0; i < 18; ++i)
			{
				const auto data = game::call<char*>(0x7FF6C7437BA0, &msg, buffer, sizeof buffer);

				if (!*data)
					break;

				if (const auto args = utils::string::split(data, ' '); args.size() > 2)
				{
					players_info.emplace_back(player_info{ i, utils::string::replace_all(utils::string::join(args, 2), "\"", "") });
				}
			}

			players[from] = std::move(players_info); 
			
			return true;
		});
	}
}