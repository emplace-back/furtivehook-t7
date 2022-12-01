#include "dependencies/std_include.hpp"
#include "session.hpp"

namespace session
{
	namespace
	{
		game::MatchMakingQuery query_info{};
		std::array<game::MatchMakingInfo, 1000> search_results;
		std::vector<game::MatchMakingInfo> sessions;
		
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
			const game::TaskDefinition* task_definition{ &task_lobby_search };
			
			if (game::TaskManager2_TaskGetInProgress(task_definition))
				return;

			query_info.queryId = game::SEARCH_SESSIONS_BY_SERVER_TYPE;
			query_info.serverType = 2000;

			if (const auto matchmaking = game::call<uintptr_t>(game::base_address + 0x144A2D0, 0); matchmaking)
			{
				game::bdRemoteTask* remote_task{};

				game::call<game::bdRemoteTask*>(game::base_address + 0x290A2B0,
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
				const auto xnaddr = *reinterpret_cast<const game::XNADDR*>(session.info.hostAddr);
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
						command::execute("join " + xuid);
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

	void initialize()
	{
		// Setup query info
		game::call(game::base_address + 0x144C2E0, &query_info, 0, true);

		// Setup search results
		for (size_t i = 0; i < search_results.size(); ++i)
			game::call(game::base_address + 0x144BA00, &search_results[i], true);
	}
}