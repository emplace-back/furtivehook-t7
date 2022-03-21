#include "dependencies/std_include.hpp"
#include "session.hpp"

namespace session
{
	namespace
	{
		std::vector<game::MatchMakingInfo> sessions;

		bool can_add_to_session(const game::MatchMakingInfo& info)
		{
			for (const auto& session : sessions)
			{
				if (session.xuid == info.xuid)
					return false;
			}

			return true;
		}
	}

	void register_session(const game::MatchMakingInfo& session)
	{
		if (!session::can_add_to_session(session))
		{
			return;
		}
			
		sessions.emplace_back(session);
	}

	void draw_session_list(const float width, const float spacing)
	{
		if (ImGui::BeginTabItem("Sessions"))
		{
			static ImGuiTextFilter filter;

			ImGui::TextUnformatted("Search session");
			filter.Draw("##search_session", width * 0.85f); 

			ImGui::SetNextItemWidth(width * 0.85f);

			if (ImGui::MenuItem("Delete sessions", nullptr, nullptr, !sessions.empty()))
			{
				sessions.clear();
			}
			
			if (ImGui::MenuItem("Fetch servers"))
			{
				const static auto TaskManager2_CreateTask = reinterpret_cast<void*(*)(uintptr_t, const ControllerIndex_t, int, int)>(game::base_address + 0x22AF770);
				const static auto dwFindSessions = reinterpret_cast<void(*)(void*, game::MatchMakingQuery *const)>(game::base_address + 0x1437DB0);
				const static auto& task_lobbySearch = reinterpret_cast<void*>(game::base_address + 0x3022208);
				const static auto& s_lobbySearch = *reinterpret_cast<game::LobbySearch*>(game::base_address + 0x15B9D600);

				const auto task = TaskManager2_CreateTask(game::base_address + 0x3022208, 0, 0, 0);

				PRINT_LOG("%i %i", s_lobbySearch.numResults);
			}

			ImGui::Separator();
			
			ImGui::BeginColumns("Sessions", 3, ImGuiColumnsFlags_NoResize);

			ImGui::SetColumnWidth(-1, 28.0f);
			ImGui::TextUnformatted("#");
			ImGui::NextColumn();
			ImGui::TextUnformatted("Session");
			ImGui::NextColumn();
			ImGui::SetColumnOffset(-1, width - ImGui::CalcTextSize("Clients").x);
			ImGui::TextUnformatted("Clients");
			ImGui::NextColumn();

			ImGui::Separator();

			std::vector<size_t> indices{};

			for (size_t i = 0; i < sessions.size(); ++i)
			{
				indices.emplace_back(i);
			}

			std::sort(indices.begin(), indices.end(), [](const auto& a, const auto& b) { return sessions[a].info.numPlayers > sessions[b].info.numPlayers; });
			
			for (const auto& session_num : indices)
			{
				const auto session = &sessions[session_num];

				const auto xnaddr = *reinterpret_cast<game::XNADDR*>(session->info.hostAddr);
				const auto ip_string = utils::string::adr_to_string(&xnaddr);
				
				if (filter.PassFilter(ip_string))
				{
					ImGui::AlignTextToFramePadding();

					ImGui::TextUnformatted(std::to_string(session_num));

					ImGui::NextColumn();

					ImGui::PushStyleColor(ImGuiCol_Text, ImColor(200, 200, 200, 250).Value);

					ImGui::AlignTextToFramePadding();

					const auto selected = ImGui::Selectable((ip_string + "##"s + std::to_string(session_num)).data());

					ImGui::PopStyleColor();

					const auto popup = "session_popup##" + std::to_string(session_num);

					if (selected)
					{
						ImGui::OpenPopup(popup.data());
					}

					if (ImGui::BeginPopup(popup.data(), ImGuiWindowFlags_NoMove))
					{
						if (ImGui::MenuItem(ip_string))
						{
							ImGui::LogToClipboardUnformatted(ip_string);
						}

						if (ImGui::MenuItem(std::to_string(session->xuid).data()))
						{
							ImGui::LogToClipboardUnformatted(std::to_string(session->xuid));
						}

						ImGui::Separator(); 
						
						if (ImGui::MenuItem("Join session"))
						{
							game::LobbyVM_JoinEvent(0, session->xuid, game::JOIN_TYPE_PARTY);
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Freeze session"))
						{
							exploit::instant_message::send_info_response_overflow({ session->xuid });
						}
						
						ImGui::EndPopup();
					}

					ImGui::NextColumn();

					ImGui::AlignTextToFramePadding();

					ImGui::TextColored(ImColor(200, 200, 200, 250).Value, "%u/%u", session->info.numPlayers, session->info.maxPlayers);

					ImGui::NextColumn();

					if (ImGui::GetColumnIndex() == 0)
					{
						ImGui::Separator();
					}
				}
			}

			ImGui::EndColumns();
			ImGui::EndTabItem();
		}
	}
}