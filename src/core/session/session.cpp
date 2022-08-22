#include "dependencies/std_include.hpp"
#include "session.hpp"

namespace session
{
	namespace
	{
		std::vector<game::MatchMakingInfo> sessions;

		const static auto& s_lobbySearch = reinterpret_cast<game::LobbySearch*>(game::base_address + 0x15B9D600);
		
		const auto lobby_search_success_callback = [](game::TaskRecord* task)
		{
			const auto remote_task = task->remoteTask;

			if (!remote_task)
				return;

			const auto num_results = remote_task->numResults;

			if (num_results == 0)
				return;

			for (size_t i = 0; i < num_results; ++i)
			{
				const auto r = reinterpret_cast<game::MatchMakingInfo*>(game::base_address + 0x99AB490 + sizeof(game::MatchMakingInfo) * i);

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
			"LobbySearch",
			0x10,
			lobby_search_success_callback,
			lobby_search_failure_callback
		};

		void fetch_sessions()
		{
			if (game::TaskManager2_TaskGetInProgress(&task_lobby_search))
				return;
				
			game::Live_SetupMatchmakingQuery();

			if (const auto task = game::TaskManager2_CreateTask(
				&task_lobby_search,
				0,
				nullptr,
				15000))
			{
				auto payload = *reinterpret_cast<game::SessionSearchPayloadData**>(&task->payload);
				*payload = {};

				game::dwFindSessions(task, &s_lobbySearch->info);
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
				const auto xnaddr = *reinterpret_cast<const game::XNADDR*>(session.info.hostAddr);
				const auto ip_string = xnaddr.to_string(true);
				const auto xuid = std::to_string(session.xuid);
				const auto label = ip_string + "##session" + xuid;
				const auto selected = ImGui::Selectable(label);
				const auto popup = "session_popup##" + xuid;

				if (!filter.PassFilter(ip_string))
					continue;
				
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
						command::execute("xshowgamercard " + xuid);
					}

					ImGui::Separator();

					const static std::vector<std::string> join_types =
					{
						"Normal",
						"Playlist",
						"Friend",
						"Invite",
						"Party",
						"Group"
					};

					if (ImGui::BeginMenu("Join Session"))
					{
						for (size_t i = 0; i < join_types.size(); ++i)
						{
							const auto label = join_types[i] + "##" + std::to_string(i);

							if (ImGui::MenuItem(label))
							{
								game::LobbyVM_JoinEvent(0, session.xuid, static_cast<game::JoinType>(i));
							}
						}

						ImGui::EndMenu();
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Freeze session"))
					{
						exploit::instant_message::send_info_response_overflow({ session.xuid });
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
}