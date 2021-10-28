#pragma once
#include "dependencies/std_include.hpp"

#define FRIENDS_LIST "friends.json"
#define PRINT_LOG_DETAILED(format, ...)																							\
{																																\
    logger::print_log(utils::string::va("[%s] %s", __FUNCTION__, format).data(), __VA_ARGS__);									\
	logger::add_to_console(utils::string::va("[%s] %s", __FUNCTION__, format).data(), __VA_ARGS__);								\
}

#define PRINT_LOG(format, ...)																									\
{																																\
    logger::print_log(format, __VA_ARGS__);																						\
    logger::add_to_console(format, __VA_ARGS__);																				\
}

#define PRINT_MESSAGE(format, ...)																								\
{																																\
    logger::print_log(format, __VA_ARGS__);																						\
    logger::add_to_console(format, __VA_ARGS__);																				\
	game::bold_game_message(format, __VA_ARGS__);																				\
}

namespace utils
{
	std::uint32_t floating_point_to_hex(const float value);
	std::string generate_log_filename(const std::string& dir, const std::string& ext = "txt");
	std::string get_log_timestamp();

	template <typename T>
	static auto atoi(const std::string& str)
	{
		return static_cast<T>(std::atoi(str.data()));
	}

	static auto atoi(const std::string& str)
	{
		return atoi<std::uint32_t>(str);
	}

	static auto atoll(const std::string& str)
	{
		return static_cast<std::uint64_t>(std::atoll(str.data()));
	}
}
