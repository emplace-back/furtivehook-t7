#include "dependencies/std_include.hpp"
#include "dvars.hpp"

namespace exception::dvars
{
	namespace
	{
		std::unordered_map<int, callback>& get_callbacks()
		{
			static std::unordered_map<int, callback> callbacks{};
			return callbacks;
		}
	}

	void register_hook(const hook_dvar index, std::uintptr_t address, const callback& callback)
	{
		const auto* dvar = *reinterpret_cast<game::dvar_t**>(address);

		get_callbacks()[index] = [=](auto& ctx)
		{
			ctx.Rcx = reinterpret_cast<std::uintptr_t>(dvar);
			ctx.Rbx = reinterpret_cast<std::uintptr_t>(dvar);
			callback(ctx);
		};

		utils::hook::set<std::uintptr_t>(address, index);
	}
	
	bool handle_exception(const LPEXCEPTION_POINTERS ex)
	{
		const auto& callbacks = get_callbacks();
		const auto handler = callbacks.find(ex->ContextRecord->Rcx);

		if (handler == callbacks.end())
		{
			return false;
		}

		handler->second(*ex->ContextRecord);
		return true;
	}

	void initialize()
	{
		dvars::register_hook(hook_dvar::com_frame, game::base_address + 0x168EEAC0, 
			[](auto)
			{
				scheduler::execute(scheduler::pipeline::main);
			}
		);

		dvars::register_hook(hook_dvar::scr_update_frame, game::base_address + 0x168EEC50, 
			[](auto)
			{
				scheduler::execute(scheduler::pipeline::renderer);
			}
		);

		dvars::register_hook(hook_dvar::handle_packet, game::base_address + 0x1574E840,
			[](auto& ctx)
			{
				const auto stack_ptr{ utils::get_stack_pointer(ctx, 0x20) };
				if (*reinterpret_cast<uintptr_t*>(stack_ptr) != game::base_address + 0x1EF7084)
					return;

				auto msg{ *reinterpret_cast<game::msg_t*>(stack_ptr + sizeof(uint64_t) + 0x30) };
				const auto module{ *reinterpret_cast<game::LobbyModule*>(stack_ptr + sizeof(uint64_t) + 0xB0) };
				const auto netadr{ *reinterpret_cast<game::netadr_t*>(ctx.Rbp) };

				if (events::lobby_msg::callback_handle_packet(netadr, msg, module))
				{
					ctx.Rsp += 0x20;
					ctx.Rbx = *reinterpret_cast<uint64_t*>(ctx.Rsp);
					ctx.Rsp += sizeof(uint64_t) * 2; // 1 pushed arg + retaddr

					ctx.Rip = game::base_address + 0x1EF7181;
				}
			}
		);

		dvars::register_hook(hook_dvar::ui_draw_text, game::base_address + 0x1791FD70,
			[](auto& ctx)
			{
				const auto stack_ptr{ utils::get_stack_pointer(ctx, 0x20) };
				if (*reinterpret_cast<uintptr_t*>(stack_ptr) != game::base_address + 0x1F342F7)
					return;

				const auto text = reinterpret_cast<char*>(ctx.R12);
				if (text && *text)
				{
					std::string result = text;
					result = utils::string::replace_all(result, "^H", "");
					result = utils::string::replace_all(result, "^B", "");

					utils::hook::write_string(text, result);
				}
			}
		);
	}
}