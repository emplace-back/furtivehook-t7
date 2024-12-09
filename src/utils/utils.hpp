#pragma once
#include "dependencies/std_include.hpp"

namespace utils
{
	void print_log(const char* msg, ...);
	std::string get_sender_string(const game::netadr_t & from);
	std::string get_chat_message(const std::string& data, const uint64_t sender_id);

	template <typename T> T random(const T min = 0, const T max = std::numeric_limits<T>::max())
	{
		std::mt19937 engine{ std::random_device{}() };
		std::uniform_int_distribution<T> dist(min, max);

		return dist(engine);
	}
	
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

	template <class F>
	class final_action
	{
	public:
		static_assert(!std::is_reference<F>::value && !std::is_const<F>::value &&
			!std::is_volatile<F>::value,
			"Final_action should store its callable by value");

		explicit final_action(F f) noexcept : f_(std::move(f))
		{
		}

		final_action(final_action&& other) noexcept
			: f_(std::move(other.f_)), invoke_(std::exchange(other.invoke_, false))
		{
		}

		final_action(const final_action&) = delete;
		final_action& operator=(const final_action&) = delete;
		final_action& operator=(final_action&&) = delete;

		~final_action() noexcept
		{
			if (invoke_) f_();
		}

		void cancel()
		{
			invoke_ = false;
		}

	private:
		F f_;
		bool invoke_{ true };
	};

	template <class F>
	final_action<typename std::remove_cv<typename std::remove_reference<F>::type>::type>
		finally(F&& f) noexcept
	{
		return final_action<typename std::remove_cv<typename std::remove_reference<F>::type>::type>(
			std::forward<F>(f));
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