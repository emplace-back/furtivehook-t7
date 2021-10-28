#pragma once
#include "dependencies/std_include.hpp"

namespace spoof_call
{
	extern "C" void* spoof_call_stub();
	
	template <typename T, typename... Args>
	static inline auto invoke(const void* shell, Args... args) -> T
	{
		auto fn = reinterpret_cast<T(*)(Args...)>(shell);
		return fn(args...);
	}

	template<typename T, typename First = void*, typename Second = void*, typename Third = void*, typename Fourth = void*, typename... Pack>
	static inline auto do_call(const void* function, void* shell_args, const size_t arg_count, First first = nullptr, Second second = nullptr, Third third = nullptr, Fourth fourth = nullptr, Pack... pack)
	{
		if (arg_count > 4)
		{
			return invoke<T, First, Second, Third, Fourth, void*, void*, Pack...>(function, first, second, third, fourth, shell_args, nullptr, pack...);
		}
		
		return invoke<T, First, Second, Third, Fourth, void*, void*>(function, first, second, third, fourth, shell_args, nullptr);
	}

	template <typename T, typename... Args>
	static inline auto call(T(*fn)(Args...), Args... args) -> T
	{
		struct shell_args
		{
			const void* trampoline;
			void* function;
		}; 
		
		shell_args p{ game::spoof_t, reinterpret_cast<void*>(fn) };
		return do_call<T, Args...>(&spoof_call_stub, &p, sizeof...(Args), args...);
	}
}