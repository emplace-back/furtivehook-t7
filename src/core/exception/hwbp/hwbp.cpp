#include "dependencies/std_include.hpp"
#include "hwbp.hpp"

namespace exception::hwbp
{
	namespace
	{
		std::unordered_map<std::uintptr_t, callback>& get_callbacks()
		{
			static std::unordered_map<std::uintptr_t, callback> callbacks{};
			return callbacks;
		}

		int get_register_index(const CONTEXT& ctx)
		{
			for (size_t i = 0; i < sizeof(std::uint32_t); ++i)
			{
				const auto a = i * sizeof(std::uint16_t);
				const auto has_index = (ctx.Dr7 & (1 << a)) == 0;

				if (has_index)
				{
					return i;
				}
			}

			return -1;
		}
	}

	bool handle_exception(const LPEXCEPTION_POINTERS ex)
	{
		const auto& callbacks = get_callbacks();
		const auto handler = callbacks.find(ex->ContextRecord->Rip);

		if (handler == callbacks.end())
		{
			return false;
		}

		handler->second(*ex->ContextRecord);
		return true;
	}

	void register_hook(const std::uintptr_t address, const callback& callback)
	{
		utils::thread::set_registers_for_each_thread([=](auto& ctx)
		{
			const auto index{ get_register_index(ctx) };

			if (index < 0 || index >= 4)
			{
				return;
			}

			auto* debug_register = reinterpret_cast<size_t*>(&ctx.Dr0);
			debug_register[index] = address;

			ctx.Dr7 |= (1 << index * 2);
			ctx.Dr7 |= (0b00 << (16 + (index * 2) - 1)); // set condition type (16-17, 21-20, 24-25, 28-29)
			ctx.Dr7 &= ~(1 << 18 + (index * 2)); // set size (18-19, 22-23, 26-27, 30-31)
		});

		get_callbacks()[address] = callback;
	}

	void register_hook(const std::uintptr_t address, const std::function<size_t()>& callback)
	{
		hwbp::register_hook(address, [=](auto& ctx)
		{
			ctx.Rip = callback();
		});
	}

	void initialize()
	{
		exception::hwbp::register_hook(game::base_address + 0x1C623A0, [](auto& ctx)
		{
			ctx.Rax = ctx.Rsp;
			ctx.Rip += 3;

			if (game::is_invalid_material_pointer(reinterpret_cast<const char*>(ctx.Rdx)))
			{
				ctx.Rip = game::base_address + 0x1C625D3;
			}
		});

		/*hwbp::register_hook(game::base_address + 0x1EE1DB0,
			[](auto& ctx)
			{
				ctx.Rcx = ctx.Rbx;
				ctx.Rip += 3;

				if (session::grab_info_of_sessions)
				{
					ctx.Rip = game::base_address + 0x1EE1DB8;
				}

				for (size_t i = 0; i < game::s_lobbySearch.numResults; ++i)
				{
					const auto session_result = game::get_session_result(i);

					if (session_result->info.hostAddrSize != sizeof game::XNADDR)
						return;

					session::register_session(*session_result);
				}
			});*/
	}
}