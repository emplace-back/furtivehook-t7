#pragma once
#include "dependencies/std_include.hpp"

namespace utils
{
	void print_log(const char* msg, ...);
	std::string get_sender_string(const game::netadr_t & from);
	std::string get_chat_message(const std::string& data, const uint64_t sender_id);

	template <typename T> auto atoi(const std::string& str)
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
}

namespace std
{
	template <>
	struct equal_to<game::netadr_t>
	{
		using result_type = bool;

		bool operator()(const game::netadr_t& lhs, const game::netadr_t& rhs) const
		{
			return game::NET_CompareAdr(lhs, rhs);
		}
	};

	template <>
	struct hash<game::netadr_t>
	{
		size_t operator()(const game::netadr_t& x) const
		{
			return hash<uint32_t>()(*reinterpret_cast<const uint32_t*>(&x.ip[0])) ^ hash<uint16_t>()(x.port);
		}
	};
}