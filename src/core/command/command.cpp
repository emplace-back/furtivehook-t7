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

	args args::get_client()
	{
		return args{ game::Sys_GetTLS()->cmdArgs };
	}

	args args::get_server()
	{
		return args{ reinterpret_cast<game::CmdArgs*>(OFFSET(0x7FF6DBB7CE30)) };
	}
	
	void args::tokenize(const char* text, const bool eval_expressions) const
	{
		// Cmd_TokenizeStringKernel
		game::call(0x7FF6C73CF7C0, -1, 0, text, 512 - cmd_args->totalUsedArgvPool, eval_expressions, cmd_args);
	}

	void args::end_tokenize() const
	{
		// Cmd_EndTokenizedString
		game::call(0x7FF6C73CD770);
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

			a.jmp(OFFSET(0x7FF6C752AE0B));
		});

		utils::hook::jump(OFFSET(0x7FF6C752AE06), sv_execute_client_command_stub);
	}
}