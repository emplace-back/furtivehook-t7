#pragma once
#include "dependencies/std_include.hpp"

namespace utils
{
	template <typename T>
	static auto atoi(const std::string& str)
	{
		return static_cast<T>(std::atoi(str.data()));
	}

	static auto atoi(const std::string& str)
	{
		return atoi<int>(str);
	}

	static auto atoll(const std::string& str)
	{
		return static_cast<std::uint64_t>(std::atoll(str.data()));
	}
	
	void print_log(const char* msg, ...);
	std::string get_sender_string(const game::netadr_t & from);
	std::string get_chat_message(const std::string& data, const uint64_t sender_id);
}