#include "dependencies/std_include.hpp"
#include "command.hpp"

namespace command
{
	namespace
	{
		using callback = std::function<bool(const command::args&)>; 
		
		std::unordered_map<std::string, callback>& get_callbacks()
		{
			static std::unordered_map<std::string, callback> callbacks{};
			return callbacks;
		}
		
		bool __fastcall sv_execute_client_command(const char* message)
		{
			auto args = command::args::get_server();
			args.tokenize(message, true);

			if (events::server_command::log_commands)
			{
				PRINT_LOG("Received SV Command: %s", args.join().data());
			}

			const auto& handlers = get_callbacks();
			const auto handler = handlers.find(utils::string::to_lower(args[0]));

			if (handler == handlers.end())
				return false;

			const auto handled = handler->second(args);

			if (handled)
			{
				args.end_tokenize();
				args.tokenize("");
			}

			return handled;
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
	
	void initialize()
	{
		const auto sv_execute_client_command_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(rcx);
			a.mov(rcx, rdx);
			a.call_aligned(command::sv_execute_client_command);
			a.pop(rcx);

			a.jmp(game::base_address + 0x2249E0B);
		});

		utils::hook::jump(game::base_address + 0x2249E06, sv_execute_client_command_stub);
	}
}