#pragma once
#include "dependencies/std_include.hpp"

namespace esp
{
	struct player_bone_tags_t
	{
		Vec2 pos[static_cast<std::uint32_t>(game::bone_tag::num_tags)];
		ImVec4 color;
	};

	struct player_box_t
	{
		int health;
		Vec2 pos;
		Vec2 scale;
	};

	struct player_name_t
	{
		std::string name;
		Vec2 pos;
		Vec3 center;
	};
	
	constexpr static auto window_title = "visuals";
	constexpr static auto window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs;
	
	extern float bone_thickness;
	extern bool enabled, player_name_tag, player_box, player_bone_tags, aimbot_positions, enemies_only;
	extern ImVec4 enemy_visible_color, enemy_color, friendly_color;
	
	void visuals(); 
	void draw();
}