#include "dependencies/std_include.hpp"
#include "server_command.hpp"

namespace events::server_command
{
	bool log_commands = true;

	namespace
	{
		using callback = std::function<bool(const command::args&)>; 
		
		std::unordered_map<uint8_t, callback>& get_callbacks()
		{
			static std::unordered_map<uint8_t, callback> callbacks{};
			return callbacks;
		}

		void on_command(const uint8_t command, const callback& callback)
		{
			get_callbacks()[command] = callback;
		}
	}

	bool handle_command(const char* buffer)
	{
		game::Cmd_TokenizeStringNoEval(buffer);
		
		const command::args args{}; 

		if (log_commands)
		{
			PRINT_LOG("Received SV Command: %s", args.join().data());
		}

		const auto& handlers = get_callbacks();
		const auto handler = handlers.find(args[0][0]);

		if (handler == handlers.end())
			return false;

		const auto handled = handler->second(args);
		
		if (handled)
		{
			game::Cmd_EndTokenizedString();
			game::Cmd_TokenizeStringNoEval("");
		}

		return handled;
	}

	void initialize()
	{
		const auto cl_cgame_needs_server_command_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(rcx);
			a.mov(rcx, rdi);
			a.call_aligned(server_command::handle_command);
			a.pop(rcx);
			
			a.jmp(game::base_address + 0x131ED18);
		});

		utils::hook::jump(game::base_address + 0x131ED0B, cl_cgame_needs_server_command_stub);
		
		server_command::on_command('2', [=](const auto& args)
		{
			const auto index = utils::atoi(args[1]);

			if (index == 3627)
			{
				const auto ignore =
				{
					"mspreload"s,
					"msload"s,
				}; 
				
				const auto result = std::any_of(ignore.begin(), ignore.end(), [=](const auto& string) { return string == utils::string::to_lower(args[2]); });

				if (!result)
				{
					return false;
				}
				
				PRINT_MESSAGE("Server", "Loadside attempt caught!");
				return true;
			}

			return false;
		});

		server_command::on_command('v', [](const auto& args)
		{
			if (args.size() != 2)
				return true;
				
			const auto argugments = utils::string::split(args.join(1), ' ');
			const auto sender_id = utils::atoll(argugments[0]);
			const auto message = utils::get_chat_message(utils::string::join(argugments, 3), sender_id);

			PRINT_LOG("%s", message.data());

			return false;
		});
	}
}