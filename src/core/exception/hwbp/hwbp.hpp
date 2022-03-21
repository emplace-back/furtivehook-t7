#pragma once
#include "dependencies/std_include.hpp"

namespace exception::hwbp
{
	bool handle_exception(const LPEXCEPTION_POINTERS ex);
	void initialize();
}