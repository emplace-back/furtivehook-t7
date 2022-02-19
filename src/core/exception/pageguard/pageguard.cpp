#include "dependencies/std_include.hpp"
#include "pageguard.hpp"

namespace exception::pageguard
{
	namespace
	{
		std::unordered_map<std::uintptr_t, callback>& get_callbacks()
		{
			static std::unordered_map<std::uintptr_t, callback> callbacks{};
			return callbacks;
		}

		void register_hook(const std::uintptr_t address, const callback& callback)
		{
			get_callbacks()[address] = callback;
		}

		void register_hook(const dvars::hook_dvar index, const std::uintptr_t dvar_address, const std::uintptr_t address, const callback& callback)
		{
			dvars::register_hook(index, dvar_address, [=](auto)
			{
				pageguard::page_guard_address(address);
			});

			register_hook(address, callback);
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

		handler->second();
		return true;
	}
	
	void page_guard_address(const std::uintptr_t address)
	{
		DWORD old_protect{ PAGE_EXECUTE | PAGE_GUARD };
		VirtualProtect(reinterpret_cast<void*>(address), sizeof std::uint8_t, old_protect, &old_protect);
	}
	
	void initialize()
	{
		pageguard::register_hook(dvars::hook_dvar::cg_predict_playerstate, game::base_address + 0x53DD170, game::base_address + 0x9C2AF0, events::cg_predict_playerstate);
	}
}