#include "dependencies/std_include.hpp"
#include "command.hpp"

namespace command
{
	args_ args;
	
	const char* args_::get(const int index) const noexcept
	{
		if (index >= this->size())
		{
			return "";
		}
		
		const auto args = game::Sys_GetTLS()->cmdArgs;
		return args->argv[args->nesting][index];
	}

	int args_::size() const noexcept
	{
		const auto args = game::Sys_GetTLS()->cmdArgs;
		return args->argc[args->nesting];
	}

	std::string args_::join(const int index) const noexcept
	{
		auto result = ""s;

		for (auto i = index; i < this->size(); ++i)
		{
			if (i > index) result.append(" ");
			result.append(this->get(i));
		}

		return result;
	}
}