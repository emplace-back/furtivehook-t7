#pragma once
#include "dependencies/std_include.hpp"

namespace rendering::dx
{
	void on_every_frame(); 
	void on_frame(IDXGISwapChain* swap_chain);
}
