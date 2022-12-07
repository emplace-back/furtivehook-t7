#include "dependencies/std_include.hpp"
#include "command.hpp"

namespace command
{
	std::unordered_map<std::string, callback> callbacks; 

	namespace
	{
		void main_handler()
		{
			const auto args = command::args::get_client();
			const auto command_lwr = utils::string::to_lower(args[0]);

			if (callbacks.find(command_lwr) != callbacks.end())
			{
				callbacks[command_lwr](args);
			}
		}

		void add_raw(const char* name, void(*callback)())
		{
			game::Cmd_AddCommandInternal(name, callback, utils::memory::get_allocator()->allocate<game::cmd_function_s>());
		}

		void add(const char* name, const callback& callback)
		{
			const auto command_lwr = utils::string::to_lower(name);

			if (callbacks.find(command_lwr) == callbacks.end())
				add_raw(name, main_handler);

			callbacks[command_lwr] = callback;
		}
	}

	void args::tokenize(const char* text, const bool eval_expressions)
	{
		if (auto& nesting{ ++cmd_args->nesting }; nesting < 8)
		{
			cmd_args->usedTextPool[nesting] = -cmd_args->totalUsedTextPool;
			cmd_args->localClientNum[nesting] = -1;
			cmd_args->controllerIndex[nesting] = 0;
			cmd_args->argv[nesting] = &cmd_args->argvPool[cmd_args->totalUsedArgvPool];
			cmd_args->argshift[nesting] = 0;
			cmd_args->argc[nesting] = game::call<int>(game::base_address + 0x20EE300, text, 512 - cmd_args->totalUsedArgvPool, eval_expressions, cmd_args->argv[nesting], cmd_args);
			cmd_args->totalUsedArgvPool += cmd_args->argc[nesting];
			cmd_args->usedTextPool[nesting] += cmd_args->totalUsedTextPool;
		}
	}

	void args::end_tokenize()
	{
		if (auto& nesting{ cmd_args->nesting }; nesting >= 0 && nesting < 8)
		{
			cmd_args->totalUsedArgvPool -= cmd_args->argc[nesting];
			cmd_args->totalUsedArgvPool -= cmd_args->argshift[nesting];
			cmd_args->totalUsedTextPool -= cmd_args->usedTextPool[nesting];
			--nesting;
		}
	}
	
	const char* args::get(const int index) const
	{
		if (index < 0 || index >= this->size())
		{
			return "";
		}

		return cmd_args->argv[cmd_args->nesting][index];
	}

	int args::size() const
	{
		return cmd_args->argc[cmd_args->nesting];
	}

	std::string args::join(const int index) const
	{
		if (index < 0)
		{
			return "";
		}
		
		std::string result{};

		for (auto i = index; i < this->size(); ++i)
		{
			if (i > index) result.append(" ");
			result.append(this->get(i));
		}

		return result;
	}

	void execute(std::string command, const bool sync)
	{
		command += "\n";

		if (sync)
		{
			game::Cmd_ExecuteSingleCommand(0, 0, command.data(), false);
		}
		else
		{
			game::Cbuf_AddText(0, command.data());
		}
	}
}