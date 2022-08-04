#include "dependencies/std_include.hpp"
#include "hwbp.hpp"

namespace exception::hwbp
{
	namespace
	{
		using callback = std::function<void(CONTEXT&)>; 
		
		std::unordered_map<std::uintptr_t, callback>& get_callbacks()
		{
			static std::unordered_map<std::uintptr_t, callback> callbacks{};
			return callbacks;
		}

		int get_register_index(const CONTEXT& ctx)
		{
			for (size_t i = 0; i < sizeof(std::uint32_t); ++i)
			{
				const auto x = i * sizeof(std::uint16_t);
				const auto has_index = (ctx.Dr7 & (1 << x)) == 0;

				if (has_index)
				{
					return i;
				}
			}

			return -1;
		}

		void register_hook(const std::uintptr_t address, const callback& callback)
		{
			utils::thread::set_registers_for_each_thread([=](auto& ctx)
			{
				const auto index{ hwbp::get_register_index(ctx) };

				if (index < 0 || index >= 4)
				{
					return;
				}

				auto* debug_register = reinterpret_cast<size_t*>(&ctx.Dr0);
				debug_register[index] = address;

				ctx.Dr7 |= (1 << index * 2);
				ctx.Dr7 |= (0 << (16 + (index * 2) - 1)); // set condition type (16-17, 21-20, 24-25, 28-29)
				ctx.Dr7 &= ~(1 << 18 + (index * 2)); // set size (18-19, 22-23, 26-27, 30-31)
			});

			get_callbacks()[address] = callback;
		}
	}
	
	bool handle_exception(const LPEXCEPTION_POINTERS ex)
	{
		if (ex->ExceptionRecord->ExceptionCode != STATUS_SINGLE_STEP)
			return false; 
		
		const auto& callbacks = get_callbacks();
		const auto handler = callbacks.find(ex->ContextRecord->Rip);

		if (handler == callbacks.end())
		{
			return false;
		}

		handler->second(*ex->ContextRecord);
		return true;
	}
	
	void initialize()
	{
		hwbp::register_hook(game::base_address + 0x1EF5610, [](auto& ctx)
		{
			ctx.Rip = reinterpret_cast<size_t>(game::LobbyMsgRW_PackageElement);
		});	

		hwbp::register_hook(game::base_address + 0x1439600, [](auto& ctx)
		{
			ctx.Rip = reinterpret_cast<size_t>(events::instant_message::dispatch_message);
		});

		hwbp::register_hook(game::base_address + 0x134BDAD, [](auto& ctx)
		{
			if (events::connectionless_packet::handle_command(*reinterpret_cast<game::netadr_t*>(ctx.R15)))
			{
				ctx.Rip = game::base_address + 0x134C43F;
			}
			else
			{
				ctx.Rdx = ctx.R12;
				ctx.Rip += 0x3;
			}
		});

		hwbp::register_hook(game::base_address + 0x131ED33, [](auto& ctx)
		{
			if (events::server_command::handle_command())
			{
				ctx.Rip = game::base_address + 0x131EEDB;
			}
			else
			{
				ctx.Rbx = *reinterpret_cast<uint64_t*>(ctx.Rax);
				ctx.Rip += 0x3;
			}
		});
	}
}