#pragma once
#include "dependencies/std_include.hpp"

namespace exception::dvar
{
	enum exception_index
	{
		renderer = 1, 
		main = 2,
		predict_ps = 3333333,
		count = 4,
	};
	
	using callback = std::function<void(const LPEXCEPTION_POINTERS&)>;
	
	extern std::unordered_map<exception_index, callback> exceptions;
	void register_exception(const exception_index& address, const callback & callback);
}