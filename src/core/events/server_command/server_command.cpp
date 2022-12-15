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

		bool __fastcall handle_command(const char* message)
		{
			auto args = command::args::get_client();
			args.tokenize(message);

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
				args.end_tokenize();
				args.tokenize("");
			}

			return handled;
		}
	}

	void initialize()
	{
		const auto cl_cgame_needs_server_command_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(rcx);
			a.mov(rcx, rdi);
			a.call_aligned(server_command::handle_command);
			a.pop(rcx);
			
			a.jmp(OFFSET(0x7FF6C65FFD18));
		});

		utils::hook::jump(OFFSET(0x7FF6C65FFD13), cl_cgame_needs_server_command_stub);
		
		server_command::on_command('2', [=](const auto& args)
		{
			const auto index = utils::atoi(args[1]);

			if (index == 3627)
			{
				const auto strings =
				{
					"mspreload"s,
					"msload"s,
				}; 
				
				const auto ignore = std::any_of(strings.begin(), strings.end(), [=](const auto& string) { return string == utils::string::to_lower(args[2]); });

				if (!ignore)
				{
					return false;
				}
				
				PRINT_MESSAGE("Server", "Loadside attempt caught!");
				return true;
			}

			return false;
		});

		server_command::on_command('t', [](const auto& args)
		{
			const auto team = utils::atoi<uint32_t>(args[2]);

			if (team >= 21)
			{
				PRINT_MESSAGE("Server", "Exploit attempt caught!");
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