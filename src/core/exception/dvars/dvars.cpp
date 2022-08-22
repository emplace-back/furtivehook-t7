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
		if (ex->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
			return false;
		
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
		dvars::register_hook(hook_dvar::scr_update_frame, game::base_address + 0x168EEC50, 
			[](auto)
			{
				scheduler::execute(scheduler::pipeline::renderer);
			}
		);

		dvars::register_hook(hook_dvar::handle_packet, game::base_address + 0x1574E840,
			[](auto& ctx)
			{
				const auto stack{ ctx.Rsp + sizeof(uint64_t) + 0x20 };
				const auto ret_address{ *reinterpret_cast<uintptr_t*>(stack) };

				if (ret_address == game::base_address + 0x1EF7084)
				{
					const auto msg = reinterpret_cast<game::msg_t*>(stack + sizeof(uint64_t) + 0x30);
					const auto msg_backup = *msg;

					if (events::lobby_msg::handle_packet(
						*reinterpret_cast<game::netadr_t*>(ctx.Rbp),
						*msg,
						*reinterpret_cast<game::LobbyModule*>(stack + sizeof(uint64_t) + 0xB0)))
					{
						msg->type = static_cast<game::MsgType>(game::MESSAGE_TYPE_NONE);
					}
					else
					{
						*msg = msg_backup;
					}
				}
			}
		);

		dvars::register_hook(hook_dvar::draw_text, game::base_address + 0x1791FD70,
			[](auto& ctx)
			{
				const auto ret_address{ *reinterpret_cast<uintptr_t*>(ctx.Rsp + sizeof(uint64_t) + 0x20) };
				
				if (ret_address == game::base_address + 0x1F342F7)
				{
					utils::string::strip_materials(reinterpret_cast<char*>(ctx.R12));
				}
				else if (ret_address == game::base_address + 0x22785E2)
				{
					utils::string::strip_materials(reinterpret_cast<char*>(ctx.R15));
				}
			}
		);

		dvars::register_hook(hook_dvar::find_line_bounds, game::base_address + 0x1791FD48,
			[](auto& ctx)
			{
				const auto ret_address{ *reinterpret_cast<uintptr_t*>(ctx.Rsp + sizeof(uint64_t) + 0x20) };

				if (ret_address == game::base_address + 0x1F34843 || ret_address == game::base_address + 0x1F34A2B)
				{
					utils::string::strip_materials(reinterpret_cast<char*>(ctx.Rdi));
				}
			}
		);

		dvars::register_hook(hook_dvar::update_presence, game::base_address + 0x1140EBB8,
			[](auto& ctx)
			{
				if (events::no_presence)
				{
					const auto ret_address{ *reinterpret_cast<uintptr_t*>(ctx.Rsp + sizeof(uint64_t) + 0x20) };

					if (ret_address == game::base_address + 0x1E93534)
					{
						// clean up Dvar_GetInt
						ctx.Rsp += 0x20;
						ctx.Rbx = *reinterpret_cast<uint64_t*>(ctx.Rsp);
						ctx.Rsp += sizeof(uint64_t) * 2; // 1 arg + retaddr

						*reinterpret_cast<game::PresenceData*>(game::base_address + 0x1140EBE0) = {}; 
						ctx.Rip = game::base_address + 0x1E93695;
					}
				}
			}
		);

		dvars::register_hook(hook_dvar::loot_enabled, game::base_address + 0x112F2720,
			[](auto& ctx)
			{
				const auto ret_address{ *reinterpret_cast<uintptr_t*>(ctx.Rsp + sizeof(uint64_t) + 0x20) };

				if (ret_address == game::base_address + 0x1E81CAB)
				{
					// clean up Dvar_GetBool
					ctx.Rsp += 0x20;
					ctx.Rbx = *reinterpret_cast<uint64_t*>(ctx.Rsp);
					ctx.Rsp += sizeof(uint64_t) * 2; // 1 arg + retaddr

					ctx.Rax = game::LiveInventory_IsValid(0) ? std::numeric_limits<int>::max() : 0;
					ctx.Rip = game::base_address + 0x1E81CCF;
				}
			}
		);
	}
}