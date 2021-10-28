#include "dependencies/std_include.hpp"
#include "hbp.hpp"

namespace exception::hbp
{
	std::unordered_map<std::uintptr_t, callback> exceptions;

	void register_exception(const std::uintptr_t address, const callback& callback)
	{
		if (exceptions.find(address) == exceptions.end())
		{
			exceptions[address] = callback;
		}
	}
}