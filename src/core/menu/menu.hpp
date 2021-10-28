#pragma once
#include "dependencies/std_include.hpp"

namespace menu
{
	constexpr static auto window_title = "furtivehook";
	constexpr static auto console_title = "console";
	constexpr static auto color_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip;
	constexpr static auto window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	const Vec2 color_position = { 440.5f, 458.0f };
	extern bool initialized;
	extern std::vector<std::string> console_items;

	void set_style(); 
	bool is_open();
	bool begin_section(const std::string& text);
	void draw();
	void initialize(IDXGISwapChain * swap_chain);
}