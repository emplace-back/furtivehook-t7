#pragma once
#include "dependencies/std_include.hpp"

namespace events
{
	void cg_predict_playerstate();
	void initialize();

	extern bool prevent_join, update_presence;
}