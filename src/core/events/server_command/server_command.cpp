#include "dependencies/std_include.hpp"
#include "server_command.hpp"

namespace events::server_command
{
	bool log_commands = false;

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

	bool __fastcall handle_command()
	{
		const command::args args{}; 

		if (log_commands)
		{
			PRINT_LOG("Received SV Command: %s", args.join().data());
		}

		const auto& handlers = get_callbacks();
		const auto handler = handlers.find(args[0][0]);

		if (handler == handlers.end())
			return false;

		return handler->second(args);
	}

	void initialize()
	{
		server_command::on_command('2', [](const auto& args)
		{
			const auto index = utils::atoi(args[1]);
			const auto string_lwr = utils::string::to_lower(args[2]);

			if (utils::string::begins_with(args[2], "^H")
				|| utils::string::begins_with(args[2], "^B")
				|| utils::string::contains(args[2], "^H")
				|| utils::string::contains(args[2], "^B"))
			{
				PRINT_LOG("Caught!");
				return true;
			}
			
			if (index == 3627)
			{
				const auto ignore_strings = 
				{
					"mspreload"s,
					"msload"s,
				};

				if (std::find(ignore_strings.begin(), ignore_strings.end(), string_lwr) == ignore_strings.end())
				{
					return false;
				}
				
				PRINT_MESSAGE("Server", "Loadside attempt caught!");
				return true;
			}

			return false;
		});
	}
}