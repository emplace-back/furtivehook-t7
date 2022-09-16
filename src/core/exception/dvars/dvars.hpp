#pragma once
#include "dependencies/std_include.hpp"

namespace exception::dvars
{
	enum hook_dvar
	{
		handle_packet = 3,
		draw_text = 4,
		find_line_bounds = 5,
		update_presence = 6,
	}; 
	
	using callback = std::function<void(CONTEXT&)>; 
	void register_hook(const hook_dvar index, std::uintptr_t address, const callback & callback);
	bool handle_exception(const LPEXCEPTION_POINTERS ex);
	void initialize();
}