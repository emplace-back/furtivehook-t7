#include "dependencies/std_include.hpp"
#include "esp.hpp"

namespace esp
{
	float bone_thickness = 2.0f;
	bool enabled = false, player_name_tag = true, player_box = true, player_bone_tags = true, aimbot_positions = false, enemies_only = true;
	ImVec4 enemy_visible_color = ImVec4(1.0f, 1.0f, 1.0f, 0.95f), enemy_color = ImVec4(1.0f, 0.0f, 0.30f, 0.95f), friendly_color = ImVec4(0.0f, 1.0f, 1.0f, 0.95f);
	std::vector<game::bone_tag> bone_tags
	{
		game::bone_tag::helmet,
		game::bone_tag::head_end,
		game::bone_tag::head,
		game::bone_tag::neck,
		game::bone_tag::shoulder_left,
		game::bone_tag::shoulder_right,
		game::bone_tag::clavicle_left,
		game::bone_tag::clavicle_right,
		game::bone_tag::spineupper,
		game::bone_tag::spine,
		game::bone_tag::spinelower,
		game::bone_tag::hip_left,
		game::bone_tag::hip_right,
		game::bone_tag::knee_left,
		game::bone_tag::knee_right,
		game::bone_tag::wrist_left,
		game::bone_tag::wrist_right,
		game::bone_tag::wrist_twist_left,
		game::bone_tag::wrist_twist_right,
		game::bone_tag::elbow_left,
		game::bone_tag::elbow_right,
		game::bone_tag::elbow_buldge_left,
		game::bone_tag::elbow_buldge_right,
		game::bone_tag::ankle_left,
		game::bone_tag::ankle_right,
		game::bone_tag::ball_left,
		game::bone_tag::ball_right,
		game::bone_tag::mainroot,
	};

	namespace screen_positions
	{
		size_t size;
		Vec2 pos;
		player_name_t player_name[18];
		player_box_t player_box[18];
		player_bone_tags_t player_bone_tags[18];
		ImVec4 player_color[18];
	}

	void draw_health_bar(ImDrawList* draw_list, const Vec2& pos, const Vec2& dimensions, const ImVec4& color)
	{
		draw_list->AddRect({ 1.f + pos.x, 1.f + pos.y }, { 1.f + pos.x + dimensions.x, 1.f + pos.y + dimensions.y }, ImColor{ 0.f, 0.f, 0.f, 1.f });
		draw_list->AddRect({ -1.f + pos.x, 1.f + pos.y }, { -1.f + pos.x + dimensions.x, 1.f + pos.y + dimensions.y }, ImColor{ 0.f, 0.f, 0.f, 1.f });
		draw_list->AddRectFilled({ pos.x, pos.y }, { pos.x + dimensions.x, pos.y + dimensions.y }, ImColor{ color });
	}

	void draw_outlined_box(ImDrawList* draw_list, const Vec2& pos, const Vec2& scale, const Vec2& length, const ImVec4& color)
	{
		draw_list->AddRectFilled({ pos.x - 1.f, pos.y - 1.f }, { pos.x + length.x + 2.f, pos.y + 2.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x - 1.f, pos.y - 1.f }, { pos.x + 2.f, pos.y + length.y + 2.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x + scale.x - length.x - 1.f, pos.y - 1.f }, { pos.x + scale.x + 1.f, pos.y + 2.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x + scale.x - 2.f, pos.y - 1.f }, { pos.x + scale.x + 1.f, pos.y + length.y + 2.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x - 1.f, pos.y + scale.y - 2.f }, { pos.x + length.x + 2.f, pos.y + scale.y + 1.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x - 1.f, pos.y + scale.y - length.y - 1.f }, { pos.x + 2.f, pos.y + scale.y + 1.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x + scale.x - length.x - 1.f, pos.y + scale.y - 2.f }, { pos.x + scale.x + 1.f, pos.y + scale.y + 1.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x + scale.x - 2.f, pos.y + scale.y - length.y - 1.f }, { pos.x + scale.x + 1.f, pos.y + scale.y + 1.f }, ImColor(color));
	}
	
	void draw_box(ImDrawList* draw_list, const Vec2& pos, const Vec2& scale, const Vec2& length, const bool outlined, const ImVec4& color)
	{
		if (outlined)
		{
			draw_outlined_box(draw_list, { pos.x, pos.y }, scale, length, { 0.0f, 0.0f, 0.0f, 1.0f });
		}
		
		draw_list->AddRectFilled({ pos.x, pos.y }, { pos.x + length.x + 1.0f, pos.y + 1.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x, pos.y }, { pos.x + 1.f, pos.y + length.y + 1.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x + scale.x - length.x, pos.y }, { pos.x + scale.x, pos.y + 1.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x + scale.x - 1.f, pos.y }, { pos.x + scale.x, pos.y + length.y + 1.f }, ImColor(color));
		draw_list->AddRectFilled({ pos.x, pos.y + scale.y - 1.f }, { pos.x + length.x + 1.f, pos.y + scale.y }, ImColor(color));
		draw_list->AddRectFilled({ pos.x, pos.y + scale.y - length.y }, { pos.x + 1.f, pos.y + scale.y }, ImColor(color));
		draw_list->AddRectFilled({ pos.x + scale.x - length.x, pos.y + scale.y - 1.f }, { pos.x + scale.x, pos.y + scale.y }, ImColor(color));
		draw_list->AddRectFilled({ pos.x + scale.x - 1.f, pos.y + scale.y - length.y }, { pos.x + scale.x, pos.y + scale.y }, ImColor(color));
	}

	void draw_line(ImDrawList* draw_list, const Vec2& pos, const Vec2& dimensions, const ImVec4& color)
	{
		draw_list->AddLine({ pos.x, pos.y }, { dimensions.x, dimensions.y }, ImColor(color), bone_thickness);
	}

	Vec3 get_bone_tag_position(const game::centity_t* cent, const game::bone_tag t)
	{
		if (const auto dobj = game::Com_GetClientDObj(cent->nextState.number, 0); dobj)
		{
			Vec3 location[static_cast<std::uint32_t>(game::bone_tag::num_tags)]{};

			if (game::CG_DObjGetWorldTagPos(&cent->pose, dobj, game::get_tag(t), &location[static_cast<size_t>(t)]))
			{
				return location[static_cast<uint32_t>(t)];
			}
		}

		return {};
	}

	Vec3 get_target_bounds(const Vec3& bone_tag_pos)
	{
		Vec3 min{}, max{};
		min.set(std::numeric_limits<float>::max());
		max.set(std::numeric_limits<float>::lowest());

		for (size_t j = 0; j < 3; ++j)
		{
			if (bone_tag_pos[j] < min[j])
				min[j] = bone_tag_pos[j];

			if (bone_tag_pos[j] > max[j])
				max[j] = bone_tag_pos[j];
		}

		return (min + max) * 0.5f;
	}

	Vec2 get_scale(const game::centity_t* cent)
	{
		const auto top_position = game::get_top_position(cent);
		const auto top = game::get_screen_pos(top_position);
		const auto bottom = game::get_screen_pos(cent->pose.origin);

		return game::get_scale(cent, bottom.y - top.y);
	}

	bool is_valid_target(game::centity_t* cent)
	{
		if (!cent->is_alive())
			return false;

		if (cent->nextState.number == game::cg()->predictedPlayerState.clientNum)
			return false;

		return true;
	}

	Vec2 get_screen_pos(const Vec3& world_pos)
	{
		Vec2 screen_pos{};
		game::CG_WorldPosToScreenPos(&world_pos, &screen_pos);
		
		return screen_pos;
	}

	void visuals()
	{
		if (!enabled)
			return;
		
		for (size_t i = 0; i < 18; ++i)
		{
			screen_positions::player_box[i].pos.clear();
			screen_positions::player_name[i].pos.clear();
			screen_positions::player_bone_tags[i].pos->clear();
			screen_positions::player_color[i] = {};

			if (const auto cent = game::centity(i); cent && is_valid_target(cent))
			{
				if (enemies_only && !game::is_enemy(i))
					continue;

				screen_positions::player_box[i].health = game::cg()->clients[i].health;

				for (const auto& bone_tag : bone_tags)
				{
					if (const auto& bone_tag_position = get_bone_tag_position(cent, bone_tag); !bone_tag_position.empty())
					{
						const auto bounds = get_target_bounds(bone_tag_position);
						const auto center_pos = get_screen_pos(bounds);
						const auto scale = get_scale(cent);

						if (const auto center_pos = get_screen_pos(bounds); !center_pos.empty() && !scale.empty())
						{
							screen_positions::player_box[i].pos = center_pos;
							screen_positions::player_box[i].pos -= scale * 0.5f;
							screen_positions::player_box[i].scale = scale;

							screen_positions::player_name[i].pos = center_pos;
							screen_positions::player_name[i].pos.y -= scale.y * 0.5f;
						}

						screen_positions::player_bone_tags[i].pos[static_cast<std::uint32_t>(bone_tag)] = get_screen_pos(bone_tag_position);
					}
				}

				screen_positions::player_color[i] = friendly_color;
				screen_positions::player_bone_tags[i].color = friendly_color;
				
				if (game::is_enemy(i))
				{
					const auto& color = enemy_color;

					screen_positions::player_color[i] = color;
					screen_positions::player_bone_tags[i].color = color;
				}

				game::CL_GetClientName(0, i, screen_positions::player_name[i].name, sizeof screen_positions::player_name[i].name, true);
				
				screen_positions::size = i;
			}
		}
	}

	void draw_player_bone_tags(size_t ent_num, ImDrawList* draw_list)
	{
		if (!player_bone_tags)
			return; 
		
		if (const auto& pos = screen_positions::player_bone_tags[ent_num].pos; !pos->empty())
		{
			const auto& color = screen_positions::player_color[ent_num];
			
			if (player_bone_tags)
			{
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::helmet)], pos[static_cast<size_t>(game::bone_tag::head)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::head)], pos[static_cast<size_t>(game::bone_tag::neck)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::neck)], pos[static_cast<size_t>(game::bone_tag::shoulder_left)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::neck)], pos[static_cast<size_t>(game::bone_tag::shoulder_right)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::shoulder_left)], pos[static_cast<size_t>(game::bone_tag::elbow_left)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::shoulder_right)], pos[static_cast<size_t>(game::bone_tag::elbow_right)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::elbow_left)], pos[static_cast<size_t>(game::bone_tag::wrist_left)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::elbow_right)], pos[static_cast<size_t>(game::bone_tag::wrist_right)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::neck)], pos[static_cast<size_t>(game::bone_tag::spinelower)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::spinelower)], pos[static_cast<size_t>(game::bone_tag::knee_left)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::spinelower)], pos[static_cast<size_t>(game::bone_tag::knee_right)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::knee_left)], pos[static_cast<size_t>(game::bone_tag::ankle_left)], color);
				draw_line(draw_list, pos[static_cast<size_t>(game::bone_tag::knee_right)], pos[static_cast<size_t>(game::bone_tag::ankle_right)], color);
			}
			else if (aimbot_positions)
			{
				for (const auto& bone_tag : bone_tags)
				{
					draw_list->AddRectFilled({ pos[static_cast<size_t>(bone_tag)][0], pos[static_cast<size_t>(bone_tag)][1] },
						{ pos[static_cast<size_t>(bone_tag)][0] + bone_thickness, pos[static_cast<size_t>(bone_tag)][1] + bone_thickness },
						ImColor(color));
				}
			}
		}
	}
	
	void draw_player_box(size_t ent_num, ImDrawList* draw_list)
	{
		if (!player_box)
			return;

		if (const auto& pos = screen_positions::player_box[ent_num].pos; !pos.empty())
		{
			const auto& color = screen_positions::player_color[ent_num];

			const auto x = pos.x;
			const auto y = pos.y;
			const auto scale = screen_positions::player_box[ent_num].scale;
			const auto length = scale * 0.25f;

			draw_box(draw_list, { x, y }, scale, length, true, color);

			const auto health{ std::min(std::max(screen_positions::player_box[ent_num].health, 0), 100) };
			const auto factor{ static_cast<float>(health) / 100.0f };
			const auto& health_color{ ImColor::HSV((factor * 120.0f) / 360.0f, 1.f, 1.f) };

			draw_health_bar(draw_list, { x - 7, y + (scale.y - scale.y * factor) }, { 3.f, scale.y * factor }, health_color.Value);
		}
	}

	void draw_player_name(size_t ent_num, ImDrawList* draw_list)
	{
		if (!player_name_tag)
			return;

		if (const auto& pos = screen_positions::player_name[ent_num].pos; !pos.empty())
		{
			const auto& color = screen_positions::player_color[ent_num];

			const auto text = screen_positions::player_name[ent_num].name;
			const auto text_size = ImGui::CalcTextSize(text);
			const auto x = pos.x - text_size.x * 0.5f;
			const auto y = pos.y - text_size.y;

			draw_list->AddText({ 1.f + x, 1.f + y }, ImColor{ ImVec4(0.0f, 0.0f, 0.0f, 1.0f) }, text);
			draw_list->AddText({ -1.f + x, 1.f + y }, ImColor{ ImVec4(0.0f, 0.0f, 0.0f, 1.0f) }, text);
			draw_list->AddText({ x, y }, ImColor(color), text);
		}
	}

	void run()
	{
		if (enabled && game::LobbyClientLaunch_IsInGame())
		{
			const auto draw_list = ImGui::GetWindowDrawList();

			for (size_t i = 0; i <= screen_positions::size; ++i)
			{
				draw_player_box(i, draw_list);
				draw_player_name(i, draw_list);
				draw_player_bone_tags(i, draw_list);
			}
		}
	}

	void esp::draw()
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_Border, {});
		
		ImGui::SetNextWindowPos({}); 
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

		if (!ImGui::Begin(window_title, nullptr, window_flags))
		{
			ImGui::End();
			return;
		}

		ImGui::GetWindowDrawList()->PushClipRectFullScreen(); 
		
		run();

		ImGui::PopStyleColor(2);
	}
}