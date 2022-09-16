#pragma once
#include "dependencies/std_include.hpp"

namespace exception::hwbp
{
	enum condition
	{
		execute = 0,
		write = 1,
		read_write = 3
	}; 
	
	bool handle_exception(const LPEXCEPTION_POINTERS ex);}