#include "dependencies/std_include.hpp"
#include "friends.hpp"

namespace friends
{
	std::vector<friend_info> friends; 
	uint32_t NONCE{ 0x696969 };

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
		
		bool update_friend_presence(const std::vector<std::uint64_t>& recipients)
		{
			const auto result = game::get_dw_presence(recipients);

			if (!result)
			{
				return false;
			}

			const auto task_data{ game::s_presenceTaskData };

			for (size_t i = 0; i < task_data->count; ++i)
			{
				const auto info = task_data->info[i];

				friends::for_each_friend(task_data->xuids[i], true, 
					[=](const auto& time, auto& friends)
					{
						auto& response = friends.response;

						if (response.presence_data.version && !info.online)
						{
							friends.last_online = std::chrono::system_clock::to_time_t(response.last_online);
							friends::write_to_friends();

							response = {};

							PRINT_MESSAGE("Presence", "%s is offline.", friends.name.data());
						}

						if (!response.valid && info.online)
						{
							friends.last_online = std::chrono::system_clock::to_time_t(time);
							friends::write_to_friends();

							PRINT_MESSAGE("Presence", "%s is online.", friends.name.data());
						}

						game::PresenceData presence_data{};
						const auto count = game::LivePresence_Serialize(true, &presence_data, info.richPresence, std::min(info.count, 128u));

						if (count > 0)
						{
							response.valid = true; 
							response.presence_data = presence_data;
						}
					}
				);
			}

			return true;
		}

		void refresh_friend_online_status()
		{
			std::vector<std::uint64_t> recipients{};

			friends::for_each_friend(0, false,
				[&recipients](const auto& time, auto& friends)
				{
					auto& response = friends.response;

					if (response.info_response.nonce && time - response.last_online > 15s)
					{
						response.info_response = {};
					}

					recipients.emplace_back(friends.steam_id);
				}
			);

			utils::for_each_batch<std::uint64_t>(recipients, 18, [](const auto& ids)
			{
				events::instant_message::send_info_request(ids);
			});

			friends::update_friend_presence(recipients);
		}
		
		void refresh_friends()
		{
			friends.clear();

			const auto json = load_friends();
			for (const auto& data : json)
			{
				for (const auto& element : data)
				{
					auto& info = element.get<friend_info>();
					info.response = {};

					friends.emplace_back(std::move(info));
				}
			}

			refresh_friend_online_status();
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
	}
	
	void write_to_friends()
	{
		json result{};
		result["friends"] = friends;

		const auto friends = get_friends_file();
		utils::io::write_file(friends, result.dump());
	}

	void for_each_friend(const uint64_t sender_id, const bool ignore, const callback& callback)
	{
		for (auto& f : friends)
		{
			if (f.steam_id != sender_id && ignore)
				continue;

			callback(std::chrono::system_clock::now(), f);
		}
	}

	void draw_friends_list(const float width, const float spacing)
	{
		if (ImGui::BeginTabItem("Friends"))
		{
			static ImGuiTextFilter filter;
			ImGui::TextUnformatted("Search friends");
			filter.Draw("##search_friend", "Name", width * 0.85f);

			ImGui::SameLine(0.0f, spacing);

			const auto popup = "Add friend##add_friend_popup"s; 
			
			if (ImGui::Button("Add friend"))
			{
				ImGui::OpenPopup(popup.data());
			}

			if (ImGui::BeginPopupModal(popup.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				static auto name_input = ""s;
				static auto steam_id_input = ""s;

				ImGui::SetNextItemWidth(width * 0.5f);
				ImGui::InputTextWithHint("##name_input", "Name", &name_input);
				
				ImGui::SetNextItemWidth(width * 0.5f);
				ImGui::InputTextWithHint("##steam_id_input", "Steam ID", &steam_id_input);

				if (ImGui::MenuItem("Add friend", nullptr, nullptr, !name_input.empty() && !steam_id_input.empty()))
				{
					friends.emplace_back(friend_info{ utils::atoll(steam_id_input), name_input });
					friends::write_to_friends();

					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Refresh friends"))
				{
					friends::refresh_friends();

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
				if (auto& f{ friends[friend_num] }; filter.PassFilter(f.name))
				{
					const auto response{ f.response };
					
					const auto presence{ response.presence_data };
					const auto info_response{ response.info_response };
					
					auto party_session{ info_response.lobby[0] };
					auto lobby_session{ info_response.lobby[1] };
					
					ImGui::AlignTextToFramePadding();

					ImGui::TextUnformatted(std::to_string(friend_num));

					ImGui::NextColumn();

					ImGui::PushStyleColor(ImGuiCol_Text, ImColor(200, 200, 200, 250).Value);

					ImGui::AlignTextToFramePadding();
					const auto selected = ImGui::Selectable(f.name.data() + "##"s + std::to_string(friend_num));

					ImGui::PopStyleColor();
					
					const auto is_local{ party_session.isValid && party_session.hostXuid == f.steam_id };

					if (is_local && party_session.hostName != f.name)
					{
						ImGui::SameLine(0, spacing);
						ImGui::TextColored(ImColor(200, 200, 200, 250).Value, "(%s)", party_session.hostName);
					}

					const auto popup = "friend_popup##" + std::to_string(friend_num);
					static game::netadr_t netadr{};

					if (selected)
					{
						game::oob::register_remote_addr(party_session, &netadr);
						
						ImGui::OpenPopup(popup.data());
					}

					if (ImGui::BeginPopup(popup.data(), ImGuiWindowFlags_NoMove))
					{
						ImGui::MenuItem(f.name + "##" + std::to_string(friend_num) + "friend_menu_item", nullptr, false, false);

						if (ImGui::IsItemClicked())
						{
							game::LiveSteam_PopOverlayForSteamID(f.steam_id);
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
								if (f.name != rename_friend_input)
								{
									f.name = rename_friend_input;
									
									write_to_friends();
								}
							}

							ImGui::EndMenu();
						}

						if (ImGui::MenuItem("Remove"))
						{
							remove_friend(f.steam_id);
						}

						ImGui::Separator();

						if (ImGui::MenuItem(std::to_string(f.steam_id)))
						{
							ImGui::LogToClipboardUnformatted(std::to_string(f.steam_id));
						}
						
						if (party_session.isValid)
						{
							ImGui::Separator(); 
							
							auto message{ "Party: " + party_session.serializedAdr.xnaddr.to_string(true) };
							message.append(" - "s + party_session.hostName + " (" + std::to_string(party_session.hostXuid) + ")");

							if (const auto party{ presence.title.party }; 
								ImGui::BeginMenu(message, party.availableCount > 1))
							{
								for (size_t i = 0; i < party.availableCount; ++i)
								{
									std::string gamertag = party.members[i].gamertag;
									if (gamertag[0] == '&' && gamertag[1] == '&' && !gamertag[2])
										gamertag = f.name;
									
									ImGui::MenuItem(gamertag + "##"s + std::to_string(i), i == 0 ? "(Leader)" : "");
								}
								
								ImGui::EndMenu();
							}
						}

						if (lobby_session.isValid && lobby_session.hostXuid != f.steam_id)
						{
							const auto message{ "Session: " + lobby_session.serializedAdr.xnaddr.to_string(true) };
							
							if (ImGui::MenuItem(message))
							{
								ImGui::LogToClipboardUnformatted(message);
							}
						}

						const auto activity = presence.title.activity;
						const auto gametype_id = presence.title.gametypeID;
						const auto map_id = presence.title.mapID;

						if (activity == game::PRESENCE_ACTIVITY_MP_PLAYING_GMODE_ON_MAP
							&& gametype_id >= 0
							&& map_id >= 0)
						{
							const auto data = game::get_gametype_on_mapname(map_id, gametype_id);
							if (!data.empty() && ImGui::MenuItem(data))
							{
								ImGui::LogToClipboardUnformatted(data);
							}
						}
						
						if (const auto start_up_time = presence.title.startupTimestamp; presence.title.startupTimestamp)
						{
							ImGui::Separator(); 
							
							ImGui::MenuItem("Online since: " + get_timestamp(start_up_time));
						}

						ImGui::Separator();
						
						if (ImGui::MenuItem("Join session"))
						{
							game::LobbyVM_JoinEvent(0, f.steam_id, game::JOIN_TYPE_PARTY);
						}

						ImGui::Separator();

						const auto is_ready{ response.valid && party_session.isValid && netadr.inaddr };
						
						if (ImGui::MenuItem("Crash game"))
						{
							exploit::instant_message::send_friend_message_crash({ f.steam_id });
							exploit::instant_message::send_info_response_overflow({ f.steam_id });
						}
						
						if (ImGui::MenuItem("Open popup", nullptr, nullptr, response.valid))
						{
							exploit::instant_message::send_popup({ f.steam_id });
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
					
					const auto timestamp = f.last_online ? get_timestamp(f.last_online) : "Never";
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
		scheduler::on_dw_initialized([]()
		{
			scheduler::once(refresh_friend_online_status, scheduler::pipeline::backend);
			scheduler::loop(refresh_friend_online_status, scheduler::pipeline::backend, 10s);
		}, scheduler::pipeline::main);
		
		friends::refresh_friends();
	}
}
