#include "dependencies/std_include.hpp"
#include "friends.hpp"

namespace friends
{
	bool fetch{ false };
	std::vector<friend_info> friends; 
	uint32_t NONCE{ 0x696969 };

	namespace
	{
		std::array<std::pair<std::vector<uint64_t>, game::dwPresenceTask*>, 256> presence_tasks;
		game::dwPresenceTask* pending_task{};
		
		const auto presence_update_success_callback = [](game::TaskRecord* task)
		{
			if (pending_task == task->payload)
				pending_task = nullptr;
			
			const auto remote_task = task->remoteTask;

			if (!remote_task)
				return;

			const auto num_results = remote_task->numResults;

			if (num_results == 0)
				return;

			const auto presence_task = std::find_if(presence_tasks.begin(), presence_tasks.end(), [=](const auto& presence) { return presence.second == task->payload; });

			if (presence_task == presence_tasks.end())
				return;

			const auto task_data = presence_task->second;

			for (size_t i = 0; i < num_results; ++i)
			{
				const auto xuid = task_data->xuids[i];
				const auto info = task_data->infos[i];

				if (const auto f = friends::get(xuid); f)
				{
					auto& presence = f->response.presence;
				
					presence = {};

					game::LivePresence_Serialize(true, &presence, info.richPresence, info.count); 
					
					if (presence.version)
					{
						f->last_online = std::time(nullptr);
					}
				}
			}

			friends::write();
		};

		const auto presence_update_failure_callback = [](game::TaskRecord* task)
		{
			if (pending_task == task->payload)
				pending_task = nullptr;

			PRINT_LOG("Presence update task failed.");
		};
		
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

			const auto result = json::parse(data);
			if (result.empty() || !result.is_object())
			{
				return {};
			}

			return result;
		}

		void update_presence(const size_t index, const std::vector<std::uint64_t>& recipients)
		{
			auto& task = presence_tasks[index];
			
			task.first = recipients;
			task.second = game::s_presenceTaskData;
			
			pending_task = task.second;
			
			pending_task->xuids = task.first.data();
			pending_task->count = static_cast<int>(task.first.size());
			pending_task->infos = reinterpret_cast<game::bdRichPresenceInfo*>(game::base_address + 0x113C8650);
			pending_task->successCallback = presence_update_success_callback;
			pending_task->failureCallback = presence_update_failure_callback;

			if (const auto result = game::dwPresenceGet(0, pending_task))
			{
				game::TaskManager2_SetupNestedTask(game::task_livepresence_dw_get, 0, result, pending_task);
			}
		}

		void fetch_sessions(const std::vector<std::uint64_t>& targets)
		{
			if (!fetch)
				return;

			std::vector<uint64_t> online_targets;
			online_targets.reserve(targets.size());

			for (const auto& id : targets)
			{
				if (const auto f = friends::get(id))
				{
					events::instant_message::send_info_request(id, friends::NONCE);
				}
			}
		}

		void update_online_status()
		{
			static std::vector<std::uint64_t> recipients{};

			if (recipients.empty())
				for (const auto& f : friends) recipients.emplace_back(f.steam_id);

			if (recipients.empty())
				return;

			if (pending_task)
				return;

			constexpr auto interval{ 10s };
			const auto now{ std::chrono::high_resolution_clock::now() };
			static std::chrono::high_resolution_clock::time_point last_time{};

			if (last_time + interval >= now)
				return;

			constexpr auto num{ 16 }; 
			
			static size_t batch_index{ 0 };

			if (batch_index * num < recipients.size())
			{
				const std::vector<uint64_t> batch
				{
					recipients.begin() + (batch_index * num),
					recipients.begin() + std::min(recipients.size(), (batch_index * num) + num)
				};

				friends::update_presence(batch_index, batch);
				friends::fetch_sessions(batch);

				++batch_index;
			}
			else
			{
				last_time = now; 
				batch_index = 0;
				recipients.clear();
			}
		}
		
		void read()
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
		}

		void remove(const std::uint64_t id)
		{
			friends.erase(std::remove_if(friends.begin(), friends.end(), [&id](const auto& f) { return f.steam_id == id; }));
			friends::write();
		}
	}

	friend_info* get(const uint64_t id)
	{
		const auto entry = std::find_if(friends.begin(), friends.end(), [id](const auto& f) { return f.steam_id == id; });

		if (entry != friends.end())
		{
			return &*entry;
		}

		return nullptr;
	}
	
	void write()
	{
		json result{};
		result["friends"] = friends;

		const auto friends = get_friends_file();
		utils::io::write_file(friends, result.dump());
	}

	void draw_friends_list(const float width, const float spacing)
	{
		if (ImGui::BeginTabItem("Friends"))
		{
			ImGui::Checkbox("Fetch sessions", &friends::fetch); 
			
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
					friends::write();

					ImGui::CloseCurrentPopup();
				}

				if (ImGui::IsKeyPressedWithIndex(ImGuiKey_Escape))
				{
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			ImGui::Separator();
			
			ImGui::BeginColumns("Friends", 2, ImGuiColumnsFlags_NoResize);

			ImGui::TextUnformatted("Friend");
			ImGui::NextColumn();
			ImGui::TextUnformatted("Online Status");
			ImGui::NextColumn();

			ImGui::Separator();

			std::vector<friend_info> sorted_friends{ friends };
			std::sort(sorted_friends.begin(), sorted_friends.end());

			for (const auto& f : sorted_friends)
			{
				if (!filter.PassFilter(f.name))
					continue;
				
				const auto xuid = std::to_string(static_cast<int64_t>(f.steam_id));
				const auto label = f.name + "##friend" + xuid;
				
				ImGui::AlignTextToFramePadding(); 
				
				const auto selected = ImGui::Selectable(label);
				
				const auto popup = "friend_popup##" + xuid;
				
				if (selected)
					ImGui::OpenPopup(popup.data());

				if (ImGui::BeginPopup(popup.data(), ImGuiWindowFlags_NoMove))
				{
					ImGui::MenuItem(f.name + "##friend_menu_item" + xuid, nullptr, false, false);

					if (ImGui::IsItemClicked())
					{
						command::execute("xshowgamercard " + xuid);
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
					}

					if (ImGui::MenuItem("Remove"))
					{
						remove(f.steam_id);
					}
					
					if (ImGui::BeginMenu("Rename##" + xuid))
					{
						auto& name = friends::get(f.steam_id)->name;

						if (ImGui::InputTextWithHint("##" + xuid, "Name", &name))
						{
							friends::write();
						}

						ImGui::EndMenu();
					}
					
					ImGui::Separator();

					if (ImGui::MenuItem(xuid))
					{
						ImGui::LogToClipboardUnformatted(xuid);
					}

					const auto response = f.response;
					const auto presence = response.presence;
					const auto info_response = response.info_response;
					const auto party_session = info_response.lobby[0];
					const auto lobby_session = info_response.lobby[1];
					
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
							game::connect_to_session(game::HostInfo{}.from_lobby(lobby_session));
						}
					}

					if (const auto start_up_time{ presence.title.startupTimestamp }; start_up_time)
					{
						ImGui::Separator();

						ImGui::MenuItem("Online since: " + utils::string::data_time(start_up_time, false));
					}
					
					ImGui::Separator();

					if (ImGui::MenuItem("Join session"))
					{
						command::execute("join " + xuid);
					}

					ImGui::Separator();
					
					game::netadr_t netadr{};
					game::net::oob::register_remote_addr(game::HostInfo{}.from_lobby(party_session), &netadr);
					const auto status = game::call<game::bdDTLSAssociationStatus>(game::base_address + 0x143BAB0, &netadr);

					const auto is_ready{ f.is_online() && status == game::BD_SOCKET_CONNECTED };

					if (ImGui::MenuItem("Crash game", nullptr, nullptr, is_ready))
					{
						exploit::send_crash(netadr);
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

					if (ImGui::BeginMenu("Send OOB##" + xuid, is_ready))
					{
						static auto oob_input = ""s;

						ImGui::InputTextWithHint("##" + xuid, "OOB", &oob_input);

						if (ImGui::MenuItem("Send OOB", nullptr, nullptr, !oob_input.empty()))
						{
							game::net::oob::send(netadr, oob_input);
						}

						ImGui::EndMenu();
					}
					
					ImGui::EndPopup();
				}

				ImGui::NextColumn();

				ImGui::AlignTextToFramePadding();

				const auto is_online = f.is_online();
				const auto timestamp = f.last_online ? utils::string::data_time(f.last_online, false) : "Never";
				const auto online_status = is_online ? "Online" : "Last seen: " + timestamp;
				
				ImGui::TextColored(is_online ? ImColor(0, 255, 127, 250).Value : ImColor(200, 200, 200, 250).Value, online_status.data());

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
		scheduler::on_dw_initialized([]()
		{
			scheduler::once(update_online_status, scheduler::pipeline::backend);
			scheduler::loop(update_online_status, scheduler::pipeline::backend);
		}, scheduler::pipeline::main);

		try
		{
			friends::read();
			friends::write();
		}
		catch (const std::exception& ex)
		{
			PRINT_LOG("Could not read 'friends.json' (%s)", ex.what());
		}
	}
}
