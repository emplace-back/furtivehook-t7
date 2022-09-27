#pragma once
#include "dependencies/std_include.hpp"

namespace utils::hwbp
{
	enum condition
	{
		execute = 0,
		write = 1,
		read_write = 3
	};

	void activate(const uintptr_t address, const condition cond, const size_t length, CONTEXT& ctx);
	void activate(const uintptr_t address, const uint32_t thread_id, const condition cond = condition::execute, const size_t length = sizeof uint8_t);
	void activate(const uintptr_t address, const condition cond = condition::execute, const size_t length = sizeof uint8_t);
	
	void deactivate(const uint32_t index, CONTEXT& ctx);
	void deactivate(const uint32_t index, const uint32_t thread_id);
	void deactivate(const uint32_t index);
	void deactivate_all(const uint32_t thread_id);
	void deactivate_all();
}