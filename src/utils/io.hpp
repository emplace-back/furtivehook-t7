#pragma once
#include "dependencies/std_include.hpp"

namespace utils::io
{
	std::string get_json_file(const std::string & name);
	std::ifstream read_json_file(const std::string & name);
	json parse_json_file(const std::string& name);
	bool write_file(const std::string & file, const std::string & data, const bool append = false);
	void write_minidump(const LPEXCEPTION_POINTERS ex, const std::string & filename, const std::string & error = {});
	bool create_directory(const std::string & directory);
	bool remove_file(const std::string & file);
	bool move_file(const std::string & src, const std::string & target);
	bool file_exists(const std::string & file);
}