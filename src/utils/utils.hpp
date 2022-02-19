#pragma once
#include "dependencies/std_include.hpp"

#define FRIENDS_LIST "friends.json"

#define PRINT_LOG_DETAILED(format, ...)																							\
{																																\
    logger::print_log(utils::string::va("[%s] %s", __FUNCTION__, format).data(), __VA_ARGS__);									\
																																\
}

#define PRINT_LOG(format, ...)																									\
{																																\
    logger::print_log(format, __VA_ARGS__);																						\
}																																\

#define PRINT_MESSAGE(title, format, ...)																						\
{																																\
    PRINT_LOG(format, __VA_ARGS__);																								\
    utils::toast::add_toast(title, utils::string::va(format, __VA_ARGS__));														\
}

namespace utils
{
	std::string get_log_file_timestamp(); 
	std::string generate_log_filename(const std::string& dir, const std::string& ext = "log");

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

	template <typename T>
	std::vector<std::vector<T>> get_batch(const std::vector<T>& v, const size_t num)
	{
		std::vector<std::vector<T>> batches{};
		batches.reserve(v.size() / num);

		for (size_t i = 0; i < v.size(); i += num)
		{
			const auto last{ std::min(v.size(), i + num) };
			batches.emplace_back(v.begin() + i, v.begin() + last);
		}

		return batches;
	}

	template <typename T>
	void for_each_batch(const std::vector<T>& v, const size_t num, const std::function<void(std::vector<T>)>& callback)
	{
		const auto batches{ get_batch(v, num) };

		for (const auto& batch : batches)
		{
			callback(batch);
		}
	}
}
