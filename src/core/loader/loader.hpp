#pragma once
#include "dependencies/std_include.hpp"

namespace loader
{
	using callback = std::function<void*(const std::string&)>; 
	
	void on_import(const std::string & library, const callback& callback);
	void load_library(const std::string & filename);
}