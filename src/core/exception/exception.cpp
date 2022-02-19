#include "dependencies/std_include.hpp"
#include "exception.hpp"

namespace exception
{
	namespace
	{
		using callback = std::function<void(CONTEXT&)>; 
		std::unordered_map<std::uintptr_t, callback>& get_callbacks()
		{
			static std::unordered_map<std::uintptr_t, callback> callbacks{};
			return callbacks;
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
			get_callbacks()[address] = callback;
		}
		
		thread_local struct
		{
			DWORD code = 0;
			PVOID address = nullptr;
		} exception_data;

		void reset_state()
		{
			auto error{ "Termination due to a stack overflow."s };
			if (exception_data.code != EXCEPTION_STACK_OVERFLOW)
			{
				error = utils::string::va("Exception: 0x%08X at offset 0x%llX", exception_data.code, DWORD64(exception_data.address) - game::base_address);
			}

			PRINT_LOG("%s", error.data());
			game::Com_Error(nullptr, 0, game::ERROR_DROP, error.data());
		}

		size_t get_reset_state_stub()
		{
			const auto stub = utils::hook::assemble([](auto& a)
			{
				a.sub(rsp, 0x10);
				a.or_(rsp, 0x8);
				a.jmp(reset_state);
			});

			return reinterpret_cast<size_t>(stub);
		}

		LONG __stdcall exception_filter(const LPEXCEPTION_POINTERS ex)
		{
			if (const auto code{ ex->ExceptionRecord->ExceptionCode };
				code != STATUS_INTEGER_OVERFLOW
				&& code != STATUS_FLOAT_OVERFLOW
				&& !handle_exception(ex)
				&& !dvars::handle_exception(ex)
				&& !hwbp::handle_exception(ex)
				&& !pageguard::handle_exception(ex))
			{
				utils::exception::minidump::write_minidump(ex);

				exception_data.code = ex->ExceptionRecord->ExceptionCode;
				exception_data.address = ex->ExceptionRecord->ExceptionAddress;
				ex->ContextRecord->Rip = get_reset_state_stub();
			}

			return EXCEPTION_CONTINUE_EXECUTION;
		}

		LPTOP_LEVEL_EXCEPTION_FILTER __stdcall set_unhandled_exception_filter(LPTOP_LEVEL_EXCEPTION_FILTER)
		{
			return &exception_filter;
		}
	}

	void initialize()
	{
		SetUnhandledExceptionFilter(exception_filter);
		utils::hook::jump(&SetUnhandledExceptionFilter, &set_unhandled_exception_filter, true);

		dvars::initialize();
		hwbp::initialize();
		pageguard::initialize();

		exception::register_hook(game::base_address + 0x20E1C6F, [](auto& ctx)
		{
			ctx.Rcx = reinterpret_cast<std::uintptr_t>(misc::trace_thread_info);
			ctx.Rip += sizeof(uint32_t);
		});	

		exception::register_hook(game::base_address + 0x20E112D, [](auto& ctx)
		{
			ctx.R9 = reinterpret_cast<std::uintptr_t>(misc::trace_thread_info);
			ctx.Rip += sizeof(uint32_t);
		});
	}
}