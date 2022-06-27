#pragma once
#include "dependencies/std_include.hpp"

namespace exception::dvars
{
	enum hook_dvar
	{
		com_frame = 1,
		scr_update_frame = 2,
		cg_predict_playerstate = 3,
		handle_packet = 4,
		draw_text = 5,
		ui_get_num_text_lines = 6,
		update_presence = 7,
		loot_enabled = 8,
	}; 
	
	using callback = std::function<void(CONTEXT&)>; 
	void register_hook(const hook_dvar index, std::uintptr_t address, const callback & callback);
	bool handle_exception(const LPEXCEPTION_POINTERS ex);
	void initialize();
}