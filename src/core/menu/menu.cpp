#include "dependencies/std_include.hpp"
#include "menu.hpp"

namespace menu
{
	bool initialized = false, open = true;
	ImFont* glacial_indifference_bold;
	ImFont* glacial_indifference;

	void set_fonts()
	{
		glacial_indifference_bold = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(ImGui::GetIO().Fonts->GetSecondaryFont().data(), 18.0f);
		glacial_indifference = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(ImGui::GetIO().Fonts->GetPrimaryFont().data(), 18.0f);
	}

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
		set_fonts();

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

	bool selectable(const std::string& label, bool enabled, bool noprint)
	{
		const auto result = ImGui::Selectable(label,
			false,
			ImGuiSelectableFlags_DontClosePopups | (enabled ? ImGuiSelectableFlags_None : ImGuiSelectableFlags_Disabled));

		if (result && !noprint)
		{
			utils::toast::add_toast("Game", "Executed: " + label);
		}

		return result;
	}

	void draw_player_list(const float width, const float spacing)
	{
		const auto draw_friend_name = [=](const std::string& player_name, const std::uint64_t steam_id)
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
				ImGui::EndTabItem();
				return;
			}
			
			const auto our_client_num = static_cast<std::uint32_t>(game::LobbySession_GetClientNumByXuid(game::LiveUser_GetXuid(0)));
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
				const auto target_client = session->clients[client_num].activeClient;

				if (target_client)
				{
					ImGui::AlignTextToFramePadding();
					ImGui::TextUnformatted(std::to_string(client_num).data());

					ImGui::NextColumn();

					const auto player_xuid = target_client->fixedClientInfo.xuid;
					const auto player_name = target_client->fixedClientInfo.gamertag;

					if (in_game)
					{
						const auto client = game::cg()->clients[client_num]; 

						ImGui::PushStyleColor(ImGuiCol_Text, client_num == our_client_num ? ImColor(0, 255, 127, 250).Value
							: (client.team == game::TEAM_FREE || client.team != game::cg()->clients[our_client_num].team)
							? esp::enemy_color : esp::friendly_color);
					}
					else
					{
						ImGui::PushStyleColor(ImGuiCol_Text, game::LiveUser_IsXUIDLocalPlayer(player_xuid)
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

					const auto netadr = game::get_session_netadr(target_client);

					draw_friend_name(player_name, player_xuid); 
					
					const auto popup = "player_popup##" + std::to_string(client_num);

					if (selected)
					{
						exploit::lobby_msg::send_connection_test(netadr, player_xuid);
						ImGui::OpenPopup(popup.data());
					}

					game::XNADDR xn;
					game::dwNetadrToCommonAddr(netadr, &xn, sizeof xn, nullptr);
					
					const auto ip_string = xn.to_string(true);

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
								static auto friend_player_name = ""s;

								ImGui::InputTextWithHint("##" + std::to_string(client_num), "Name", &friend_player_name);

								if (ImGui::Button("Add friend"))
								{
									friends::friends.emplace_back(friends::friend_info{ player_xuid, friend_player_name });
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

						if (ImGui::BeginMenu("Crash player##" + std::to_string(client_num)))
						{
							static auto freeze = false;
							ImGui::Checkbox("Freeze", &freeze);
							
							if (ImGui::MenuItem("Crash"))
							{
								exploit::send_crash(netadr, player_xuid, freeze);
							}

							ImGui::EndMenu();
						}

						if (ImGui::BeginMenu("Exploits##" + std::to_string(client_num)))
						{
							if (ImGui::MenuItem("Show migration screen", nullptr, nullptr, can_connect_to_player))
							{
								exploit::send_mstart_packet(netadr);
							}

							if (ImGui::MenuItem("Immobilize", nullptr, nullptr, can_connect_to_player))
							{
								exploit::send_request_stats_packet(netadr);
							}

							if (ImGui::MenuItem("Kick from lobby", nullptr, nullptr, can_connect_to_player))
							{
								exploit::send_connect_response_migration_packet(netadr);
							}

							if (ImGui::MenuItem("Disconnect client from lobby"))
							{
								exploit::lobby_msg::send_disconnect_client(player_xuid);
							}

							if (ImGui::MenuItem("Disconnect from lobby"))
							{
								exploit::lobby_msg::send_disconnect(player_xuid);
							}

							if (ImGui::BeginMenu("Send OOB##" + std::to_string(client_num), can_connect_to_player))
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

			if (ImGui::BeginTabBar("Tabs"))
			{
				ImGui::Separator();

				const auto width = ImGui::GetContentRegionAvail().x;
				const auto spacing = ImGui::GetStyle().ItemInnerSpacing.x;

				if (ImGui::BeginTabItem("Aimbot"))
				{
					ImGui::Checkbox("Enabled##aimbot_enabled", &aimbot::enabled);

					ImGui::Separator();

					ImGui::Checkbox("Silent mode", &aimbot::silent);
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
						ImGui::SetNextItemWidth(width * 0.45f);
						ImGui::InputTextWithHint("##player_name", "Name", &steam::persona_name);
					}

					ImGui::Checkbox("Log out-of-band packets", &events::connectionless_packet::log_commands);
					ImGui::Checkbox("Log instant messages", &events::instant_message::log_messages);
					ImGui::Checkbox("Log lobby messages", &events::lobby_msg::log_messages);
					ImGui::Checkbox("Log server commands", &events::server_command::log_commands);
					ImGui::Checkbox("Prevent join", &events::prevent_join);
					ImGui::Checkbox("Don't update presence", &events::update_presence);

					if (ImGui::CollapsingHeader("Removals", ImGuiTreeNodeFlags_Leaf))
					{
						ImGui::Checkbox("Remove kick angles", &misc::no_recoil);
						ImGui::Checkbox("Remove spread", &nospread::enabled);
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Server"))
				{
					if (ImGui::CollapsingHeader("Exploits", ImGuiTreeNodeFlags_Leaf))
					{
						static auto target_steam_id{ ""s };

						ImGui::SetNextItemWidth(width * 0.85f);
						ImGui::InputTextWithHint("##target_steam_id", "Steam ID", &target_steam_id); 

						const auto target_id{ utils::atoll(target_steam_id) };

						if (ImGui::MenuItem("Send crash", nullptr, nullptr, target_id && !target_steam_id.empty()))
						{
							exploit::instant_message::send_friend_message_crash({ target_id });
							exploit::instant_message::send_info_response_overflow({ target_id });
						}

						if (ImGui::MenuItem("Send popup", nullptr, nullptr, target_id && !target_steam_id.empty()))
						{
							exploit::instant_message::send_popup({ target_id });
						}

						ImGui::Separator();

						if (ImGui::MenuItem("Endgame"))
						{
							command::execute("mr " + std::to_string(game::cl()->serverId) + " 0 endround");
						}

						if (ImGui::MenuItem("Crash all"))
						{
							command::execute("loadside 0 "s + game::ui_mapname->current.string + " 0 0");
						}
						
						if (ImGui::MenuItem("Crash server"))
						{
							exploit::send_crash(game::session->host.info.netAdr, game::session->host.info.xuid);
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
								command::execute(command_input);
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
								command::execute(command);
							}
						}
					}

					ImGui::EndTabItem();
				}

				menu::draw_player_list(width, spacing);
				friends::draw_friends_list(width, spacing);
				session::draw_session_list(width, spacing);
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
		}
	}
}