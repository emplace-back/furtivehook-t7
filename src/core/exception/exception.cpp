#include "dependencies/std_include.hpp"
#include "exception.hpp"

namespace exception
{
	namespace
	{
		LONG __stdcall exception_filter(const LPEXCEPTION_POINTERS ex)
		{
			const auto code{ ex->ExceptionRecord->ExceptionCode }; 
			const auto address{ ex->ExceptionRecord->ExceptionAddress };
			
			if (code != STATUS_INTEGER_OVERFLOW
				&& code != STATUS_FLOAT_OVERFLOW
				&& !dvars::handle_exception(ex)
				&& !hwbp::handle_exception(ex)
				&& !pageguard::handle_exception(ex))
			{
				utils::exception::minidump::write(ex);

				auto error{ "Termination due to a stack overflow."s };
				if (code != EXCEPTION_STACK_OVERFLOW)
				{
					error = utils::string::va("Exception: 0x%08X at offset 0x%llX", code, reinterpret_cast<uintptr_t>(address) - game::base_address);
				}

				PRINT_LOG("%s", error.data());
				game::Com_Error(nullptr, 0, game::ERROR_DROP, error.data());
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
	}
}