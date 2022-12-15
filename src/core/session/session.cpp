#include "dependencies/std_include.hpp"
#include "session.hpp"

namespace session
{
	namespace
	{
		struct player_info
		{
			size_t client_num; 
			std::string name;
		}; 
		
		game::MatchMakingQuery query_info{};
		std::array<game::MatchMakingInfo, 1000> search_results;
		std::vector<game::MatchMakingInfo> sessions;
		std::unordered_map<game::netadr_t, std::vector<player_info>> players;

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
			game::TaskDefinition const* task_definition{ &task_lobby_search };
			
			if (game::TaskManager2_TaskGetInProgress(task_definition))
				return;

			query_info.queryId = game::SEARCH_SESSIONS_BY_SERVER_TYPE;
			query_info.serverType = 2000;

			if (const auto matchmaking = game::call<uintptr_t>(0x7FF6C672B2D0, 0); matchmaking)
			{
				game::bdRemoteTask* remote_task{};

				game::call<game::bdRemoteTask*>(0x7FF6C7BEB2B0,
					matchmaking,
					&remote_task,
					query_info.queryId,
					0,
					search_results.size(),
					&query_info,
					search_results.data());

				game::TaskManager2_SetupRemoteTask(task_definition, remote_task, 10000);
			}
		}
	}
	
	void register_session(const game::MatchMakingInfo& session)
	{
		const auto entry = std::find_if(sessions.begin(), sessions.end(), [&](const auto& s) { return s.xuid == session.xuid; });

		if (entry == sessions.end())
		{
			sessions.emplace_back(session);
		}
		else
		{
			*entry = session;
		}
	}
	
	void draw_session_list(const float width, const float spacing)
	{
		if (ImGui::BeginTabItem("Sessions"))
		{
			static ImGuiTextFilter filter;
			ImGui::TextUnformatted("Search session");
			filter.Draw("##search_session", width * 0.85f); 

			if (ImGui::MenuItem("Fetch sessions")) 
			{ 
				session::fetch_sessions();
			}

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

					if (ImGui::MenuItem("Freeze session"))
					{
						exploit::instant_message::send_info_response_overflow(session.xuid);
					}

					if (ImGui::BeginMenu("Send OOB##" + xuid))
					{
						static auto oob_input = ""s;

						ImGui::InputTextWithHint("##" + xuid, "OOB", &oob_input);

						if (ImGui::MenuItem("Send OOB", nullptr, nullptr, !oob_input.empty()))
						{
							game::net::oob::on_registered_addr(host_info, [=](const auto& netadr)
							{
								game::net::netchan::send_oob(session.xuid, netadr, oob_input);
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
							game::net::oob::on_registered_addr(host_info, [=](const auto& netadr)
							{
								game::net::netchan::send_oob(session.xuid, netadr, "getstatus\n");
							});
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
		// Setup query info
		game::call(0x7FF6C672D2E0, &query_info, 0, true);

		// Setup search results
		for (size_t i = 0; i < search_results.size(); ++i) 
			game::call(0x7FF6C672CA00, &search_results[i], true);

		events::connectionless_packet::on_command("statusResponse", [](const auto& args, const auto& target, auto& msg)
		{
			char buffer[1024]{ 0 };
			game::call(0x7FF6C7437BA0, &msg, buffer, sizeof buffer);

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

			players[target] = std::move(players_info); 
			
			return true;
		});
	}
}