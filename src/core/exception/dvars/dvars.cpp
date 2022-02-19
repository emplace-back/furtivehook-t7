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
		});

		dvars::register_hook(hook_dvar::scr_update_frame, game::base_address + 0x168EEC50, 
			[](auto)
		{
			scheduler::execute(scheduler::pipeline::renderer);
		});
	}
}