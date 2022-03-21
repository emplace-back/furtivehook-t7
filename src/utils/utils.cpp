#include "dependencies/std_include.hpp"
#include "utils.hpp"

namespace utils 
{
	uintptr_t get_stack_pointer(const CONTEXT& ctx, const int offset)
	{
		const auto stack_pointer{ ctx.Rsp + sizeof(uint64_t) + offset };
		return stack_pointer;
	}
}