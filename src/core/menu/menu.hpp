#pragma once
#include "dependencies/std_include.hpp"

namespace menu
{
	constexpr static auto window_title = "furtivehook";
	constexpr static auto console_title = "console";
	constexpr static auto color_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip;
	constexpr static auto window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
	extern bool initialized, deez;
	extern ImFont* glacial_indifference_bold;
	extern ImFont* glacial_indifference;

	void set_style(); 
	bool is_open();
	bool begin_section(const std::string& text);
	bool selectable(const std::string & label, bool enabled = true, bool noprint = false);
	void draw();
	void initialize(IDXGISwapChain * swap_chain);

	struct exploit_t
	{
		std::string chat_message;
		std::string player_name;
		bool completed;
	}; 
	
	extern std::unordered_map<uint64_t, exploit_t> exploits;
}