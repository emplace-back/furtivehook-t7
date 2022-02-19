#include "dependencies/std_include.hpp"
#include "command.hpp"

namespace command
{
	std::unordered_map<std::string, callback> callbacks; 

	namespace
	{
		void main_handler()
		{
			const args args{};
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

	const char* args::get(const size_t index) const noexcept
	{
		if (index >= this->size())
		{
			return "";
		}

		const auto args = game::Sys_GetTLS()->cmdArgs;
		return args->argv[args->nesting][index];
	}

	int args::size() const noexcept
	{
		const auto args = game::Sys_GetTLS()->cmdArgs;
		return args->argc[args->nesting];
	}

	std::string args::join(const size_t index) const noexcept
	{
		auto result = ""s;

		for (size_t i = index; i < this->size(); ++i)
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
		add("list_asset", [](const auto& args)
		{
			if (args.size() > 0)
			{
				const auto type = utils::atoi<game::XAssetType>(args[1]);

				if (type >= game::ASSET_TYPE_PHYSPRESET && type < game::ASSET_TYPE_COUNT)
				{
					PRINT_LOG("Listing assets in [%s] pool", game::g_assetNames[type]);

					game::enum_assets(type, [=](const auto& header)
					{
						const auto asset = game::XAsset{ type, header };
						const auto asset_name = game::DB_GetXAssetName(&asset);

						PRINT_LOG("%s", asset_name);
					}, true);
				}
			}
		});
	}
}