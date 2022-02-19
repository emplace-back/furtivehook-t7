#include "dependencies/std_include.hpp"
#include "friends.hpp"

namespace friends
{
	std::vector<friend_info> friends; 
	uint32_t NONCE{ 1111111111u };

	namespace 
	{
		std::string get_timestamp(const std::int64_t time)
		{
			tm ltime{};
			char timestamp[MAX_PATH] = { 0 };

			_localtime64_s(&ltime, &time);
			std::strftime(timestamp, sizeof timestamp - 1, "%D (%r)", &ltime);

			return timestamp;
		}

		void refresh_friend_online_status()
		{
			std::vector<std::uint64_t> recipients{};

			for (const auto& friends : friends::friends)
			{
				recipients.emplace_back(friends.steam_id);
			}

			utils::for_each_batch<std::uint64_t>(recipients, 18, [](const auto& steam_ids)
			{
				events::instant_message::send_info_request(steam_ids);
			});
		}

		void add_friend_response(const game::Msg_InfoResponse& info_response, friends::friend_info& friends)
		{
			response_t response{};
			response.valid = true;
			response.info_response = info_response;
			response.last_online = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

			if (!friends.response.valid)
			{
				friends.last_online = response.last_online;
				friends::write_to_friends();

				PRINT_MESSAGE("Friends", "%s is online.", friends.name.data());
			}

			friends.response = response;
		}

		void remove_friend(const std::uint64_t steam_id)
		{
			const auto entry = std::find_if(friends.begin(), friends.end(), [&steam_id](const auto& friends) { return friends.steam_id == steam_id; });

			if (entry != friends.end())
			{
				friends.erase(entry);
			}

			write_to_friends();
		}

		std::string get_friends_file()
		{
			const utils::nt::library self{};
			return self.get_folder() + "\\furtivehook\\friends.json";
		}
		
		json load_friends()
		{
			std::string data{};
			const auto friends = get_friends_file();
			if (!utils::io::read_file(friends, &data))
			{
				return {};
			}

			const auto result = json::parse(data, nullptr, false);
			if (result.empty() || !result.is_object())
			{
				return {};
			}
			
			return result;
		}

		void refresh_friends()
		{
			friends.clear();

			const auto json = load_friends();
			for (const auto& element : json) 
			{
				for (size_t i = 0; i < element.size(); ++i)
				{
					auto& info = element[i].get<friend_info>();
					info.response = {};
					
					friends.emplace_back(std::move(info));
				}
			}

			refresh_friend_online_status();
		}
	}
	
	void write_to_friends()
	{
		json result{};
		result["friends"] = friends::friends;

		const auto friends = get_friends_file();
		utils::io::write_file(friends, result.dump());
	}

	void add_friend_response(const game::Msg_InfoResponse& info_response, const std::uint64_t sender_id)
	{
		for (auto& friends : friends::friends)
		{
			if (friends.steam_id == sender_id)
			{
				add_friend_response(info_response, friends);
			}
		}
	}

	void draw_friends_list(const float width, const float spacing)
	{
		if (ImGui::BeginTabItem("Friends"))
		{
			const auto popup = "Add friend##add_friend_popup"s;

			static ImGuiTextFilter filter;
			
			ImGui::TextUnformatted("Search friends");
			filter.Draw("##search_friend", "Name", width * 0.85f);

			ImGui::SameLine(0.0f, spacing);

			if (ImGui::Button("Add friend"))
			{
				ImGui::OpenPopup(popup.data());
			}

			ImGui::SetNextWindowBgAlpha(1.0f);

			if (ImGui::BeginPopupModal(popup.data(), nullptr, ImGuiWindowFlags_NoResize))
			{
				static auto name_input = ""s;
				static auto steam_id_input = ""s;
				static auto ip_data_input = ""s;

				ImGui::AlignTextToFramePadding(); 
				
				ImGui::SetNextItemWidth(width * 0.5f);
				ImGui::InputTextWithHint("##name_input", "Name", &name_input);

				ImGui::SetNextItemWidth(width * 0.5f);
				ImGui::InputTextWithHint("##steam_id_input", "Steam ID", &steam_id_input);

				ImGui::AlignTextToFramePadding(); 
				
				ImGui::SetNextItemWidth(width * 0.5f);
				ImGui::InputTextWithHint("##ip_data_input", "IP Address", &ip_data_input);

				if (ImGui::MenuItem("Add friend", nullptr, nullptr, !name_input.empty() && !steam_id_input.empty()))
				{
					friends.emplace_back(friend_info{ utils::atoll(steam_id_input), name_input });
					write_to_friends();

					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Refresh friends"))
				{
					refresh_friends();

					ImGui::CloseCurrentPopup();
				}

				if (ImGui::IsKeyPressedWithIndex(ImGuiKey_Escape))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::Separator();
			
			ImGui::BeginColumns("Friends", 3, ImGuiColumnsFlags_NoResize);

			ImGui::SetColumnWidth(-1, 28.0f);
			ImGui::TextUnformatted("#");
			ImGui::NextColumn();
			ImGui::TextUnformatted("Friend");
			ImGui::NextColumn();
			ImGui::SetColumnOffset(-1, width * 0.55f);
			ImGui::TextUnformatted("Online Status");
			ImGui::NextColumn();

			ImGui::Separator();

			std::vector<size_t> indices{};

			for (size_t i = 0; i < friends.size(); ++i)
			{
				indices.emplace_back(i);
			}

			std::sort(indices.begin(), indices.end(), [](const auto& a, const auto& b) { return friends[a].last_online > friends[b].last_online; });
			std::sort(indices.begin(), indices.end(), [](const auto& a, const auto& b) { return friends[a].response.valid > friends[b].response.valid; });

			for (const auto& friend_num : indices)
			{
				if (auto& friends{ friends::friends[friend_num] }; filter.PassFilter(friends.name))
				{
					const auto response{ friends.response };
					const auto lobby{ response.info_response.lobby[0] };
					
					ImGui::AlignTextToFramePadding();

					ImGui::TextUnformatted(std::to_string(friend_num));

					ImGui::NextColumn();

					ImGui::PushStyleColor(ImGuiCol_Text, ImColor(200, 200, 200, 250).Value);

					ImGui::AlignTextToFramePadding();
					const auto selected = ImGui::Selectable(friends.name.data() + "##"s + std::to_string(friend_num));

					ImGui::PopStyleColor();

					const auto is_local{ lobby.hostXuid == friends.steam_id && lobby.isValid };

					if (is_local && lobby.hostName != friends.name)
					{
						ImGui::SameLine(0, spacing);
						ImGui::TextColored(ImColor(200, 200, 200, 250).Value, "(%s)", lobby.hostName);
					}

					const auto popup = "friend_popup##" + std::to_string(friend_num);
					static game::netadr_t netadr{};

					if (selected)
					{
						if (!game::oob::register_remote_addr(lobby, &netadr))
						{
							PRINT_LOG("Failed to retrieve the remote IP address from XNADDR.");
						}
						
						ImGui::OpenPopup(popup.data());
					}

					if (ImGui::BeginPopup(popup.data(), ImGuiWindowFlags_NoMove))
					{
						ImGui::MenuItem(friends.name + "##" + std::to_string(friend_num) + "friend_menu_item", nullptr, false, false);

						if (ImGui::IsItemClicked())
						{
							game::LiveSteam_PopOverlayForSteamID(friends.steam_id);
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
						}

						if (ImGui::BeginMenu("Rename##" + std::to_string(friend_num)))
						{
							static auto rename_friend_input = ""s;

							ImGui::InputTextWithHint("##" + std::to_string(friend_num), "Name", &rename_friend_input);

							if (ImGui::MenuItem("Rename"))
							{
								if (friends.name != rename_friend_input)
								{
									friends.name = rename_friend_input;
									
									write_to_friends();
								}
							}

							ImGui::EndMenu();
						}

						if (ImGui::MenuItem("Remove"))
						{
							remove_friend(friends.steam_id);
						}

						ImGui::Separator();

						if (ImGui::MenuItem(std::to_string(friends.steam_id)))
						{
							ImGui::LogToClipboardUnformatted(std::to_string(friends.steam_id));
						}

						const auto is_ready{ response.valid && netadr.inaddr && is_local };
						const auto ip_data_string{ is_ready ? utils::string::adr_to_string(&netadr) : "Invalid IP Data" };

						if (ImGui::MenuItem(ip_data_string, nullptr, nullptr, is_ready))
						{
							ImGui::LogToClipboardUnformatted(ip_data_string);
						}

						ImGui::Separator();
						
						if (ImGui::MenuItem("Join session", nullptr, nullptr, response.valid))
						{
							const auto command = "join " + std::to_string(friends.steam_id);
							command::execute(command);
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Crash game"))
						{
							exploit::instant_message::send_friend_message_crash({ friends.steam_id });
							exploit::instant_message::send_info_response_overflow({ friends.steam_id });
						}
						
						if (ImGui::MenuItem("Open popup", nullptr, nullptr, response.valid))
						{
							exploit::instant_message::send_popup({ friends.steam_id });
						}

						if (ImGui::MenuItem("Kick player", nullptr, nullptr, is_ready))
						{
							exploit::send_connect_response_migration_packet(netadr);
						}

						if (ImGui::MenuItem("Show migration screen", nullptr, nullptr, is_ready))
						{
							exploit::send_mstart_packet(netadr);
						}

						if (ImGui::MenuItem("Immobilize", nullptr, nullptr, is_ready))
						{
							exploit::send_request_stats_packet(netadr);
						}

						if (ImGui::BeginMenu("Send OOB##" + std::to_string(friend_num), is_ready))
						{
							static auto oob_input = ""s;

							ImGui::InputTextWithHint("##" + std::to_string(friend_num), "OOB", &oob_input);

							if (ImGui::MenuItem("Send OOB", nullptr, nullptr, !oob_input.empty()))
							{
								game::oob::send(netadr, oob_input);
							}

							ImGui::EndMenu();
						}

						ImGui::EndPopup();
					}

					ImGui::NextColumn();

					ImGui::AlignTextToFramePadding();
					
					const auto timestamp = friends.last_online ? get_timestamp(friends.last_online) : "Never";
					const auto online_status = response.valid ? "Online" : "Last seen: " + timestamp;
					ImGui::TextColored(response.valid ? ImColor(0, 255, 127, 250).Value : ImColor(200, 200, 200, 250).Value, online_status.data());

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

	void initialize()
	{
		scheduler::on_dw_initialized(refresh_friend_online_status, scheduler::pipeline::backend, 10s);
		friends::refresh_friends();
	}
}
