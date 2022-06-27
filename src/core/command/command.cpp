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

	const char* args::get(const int index) const noexcept
	{
		if (index < 0 || index >= this->size())
		{
			return "";
		}

		return cmd_args->argv[cmd_args->nesting][index];
	}

	int args::size() const noexcept
	{
		return cmd_args->argc[cmd_args->nesting];
	}

	std::string args::join(const int index) const noexcept
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
		command::add("list_asset", [](const auto& args)
		{
			if (args.size() > 0)
			{
				const auto type = utils::atoi<game::XAssetType>(args[1]);

				if (type >= game::ASSET_TYPE_PHYSPRESET && type < game::ASSET_TYPE_COUNT)
				{
					PRINT_LOG("Listing assets in [%s] pool", game::g_assetNames[type]);

					std::vector<std::string> asset_names{};
					
					game::enum_assets(type, [&](const auto& header)
					{
						const auto asset = game::XAsset{ type, header };
						const auto asset_name = game::DB_GetXAssetName(&asset);
						
						asset_names.emplace_back(asset_name);
					}, true);

					std::sort(asset_names.begin(), asset_names.end(), [](const auto& a, const auto& b) { return a < b; });
					
					for (const auto& name : asset_names)
					{
						PRINT_LOG("%s", name.data());
					}
				}
			}
		});
	}
}