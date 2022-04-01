#include "dependencies/std_include.hpp"
#include "session.hpp"

namespace session
{
	namespace
	{
		std::vector<game::MatchMakingInfo> sessions;

		bool can_add_to_session(const game::MatchMakingInfo& info)
		{
			for (auto& session : sessions)
			{
				if (session.xuid == info.xuid)
				{
					session = info; 
					return false;
				}
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

	void fetch_servers()
	{
		const auto result = game::get_dw_sessions();

		if (result)
		{
			for (size_t i = 0; i < 50; ++i)
			{
				const auto session = game::get_session_result(i);
				if (session->info.hostAddrSize == sizeof game::XNADDR)
				{
					session::register_session(*session);
				}
			}
		}
	}

	void draw_session_list(const float width, const float spacing)
	{
		if (ImGui::BeginTabItem("Sessions"))
		{
			static ImGuiTextFilter filter;
			ImGui::TextUnformatted("Search session");
			filter.Draw("##search_session", width * 0.85f); 

			static auto show_empty{ true };
			ImGui::MenuItem("Show empty sessions", nullptr, &show_empty);

			if (ImGui::MenuItem("Fetch sessions")) 
			{ 
				session::fetch_servers(); 
			}
	
			ImGui::BeginColumns("Sessions", 1, ImGuiColumnsFlags_NoResize);

			ImGui::NextColumn();

			std::vector<size_t> indices{};

			for (size_t i = 0; i < sessions.size(); ++i)
			{
				indices.emplace_back(i);
			}

			std::sort(indices.begin(), indices.end(), [](const auto& a, const auto& b) { return sessions[a].info.numPlayers > sessions[b].info.numPlayers; });
			
			for (const auto& session_num : indices)
			{
				const auto session = &sessions[session_num];

				if (!show_empty && session->isEmpty)
					continue;

				const auto xnaddr = *reinterpret_cast<game::XNADDR*>(session->info.hostAddr);
				const auto ip_string = xnaddr.to_string(true);
				
				if (filter.PassFilter(ip_string))
				{
					ImGui::AlignTextToFramePadding(); 

					const auto message = utils::string::va("%u/%u", session->info.numPlayers, session->info.maxPlayers);
					const auto selected = ImGui::MenuItem(ip_string + "##"s + std::to_string(session_num), message.data());

					if (session->xuid == game::session->host.info.xuid)
					{
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 0.95f), "[Current]");
					}

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

						if (ImGui::MenuItem(std::to_string(session->xuid)))
						{
							game::LiveSteam_PopOverlayForSteamID(session->xuid);
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Join session"))
						{
							game::LobbyVM_JoinEvent(0, session->xuid, game::JOIN_TYPE_PARTY);
						}

						if (ImGui::MenuItem("Join session2"))
						{
							game::LobbyVM_JoinEvent(0, session->xuid, game::JOIN_TYPE_NORMAL);
						}

						if (ImGui::MenuItem("Join session3"))
						{
							game::LobbyVM_JoinEvent(0, session->xuid, game::JOIN_TYPE_FRIEND);
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Freeze session"))
						{
							exploit::instant_message::send_info_response_overflow({ session->xuid });
						}

						ImGui::EndPopup();
					}

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