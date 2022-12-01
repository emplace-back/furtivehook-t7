#include "dependencies/std_include.hpp"
#include "exception.hpp"

namespace exception
{
	namespace
	{
		bool is_exception_in_game_module(const uintptr_t address, const uintptr_t base)
		{
			utils::nt::library self{};
			return address >= base && address < base - 0x1000 + self.get_nt_headers()->OptionalHeader.SizeOfImage;
		}
		
		LONG __stdcall exception_filter(const LPEXCEPTION_POINTERS ex)
		{
			const auto code = ex->ExceptionRecord->ExceptionCode; 
			const auto addr = reinterpret_cast<uintptr_t>(ex->ExceptionRecord->ExceptionAddress);
			const auto base = game::base_address; 
			const auto offset = addr - base;
			
			if (code != STATUS_INTEGER_OVERFLOW
				&& code != STATUS_FLOAT_OVERFLOW
				&& code != STATUS_SINGLE_STEP)
			{
				std::string message;

				if (is_exception_in_game_module(addr, base))
				{
					message = utils::string::va("Exception: 0x%08X at offset 0x%llX", code, offset);
				}
				else
				{
					message = utils::string::va("Exception: 0x%08X at 0x%llX (outside of game module)", code, addr);
				}

				PRINT_LOG("%s", message.data());

				if (code == STATUS_ACCESS_VIOLATION || code == STATUS_STACK_BUFFER_OVERRUN)
				{
					MessageBoxA(nullptr, message.data(), "Exception", MB_ICONERROR);

					if (!utils::exception::minidump::write(ex))
					{
						MessageBoxA(nullptr, utils::string::va("There was an error creating the minidump! (0x%08X)", GetLastError()).data(), "Minidump Error", MB_OK | MB_ICONERROR);
					}
					
					utils::nt::terminate(code);
				}
				
				return EXCEPTION_CONTINUE_SEARCH;
			}

			return EXCEPTION_CONTINUE_EXECUTION;
		}

		void __stdcall set_unhandled_exception_filter_stub(LPTOP_LEVEL_EXCEPTION_FILTER)
		{
			// Don't register anything here...
		}
	}

	void write_mini_dump(void*, const LPEXCEPTION_POINTERS ex)
	{
		const auto addr = reinterpret_cast<uintptr_t>(ex->ExceptionRecord->ExceptionAddress);
		const auto offset = addr - game::base_address;
		
		DEBUG_LOG("Games exception handler called."
			"\nAt offset 0x%llX (0x%llX)",
			offset,
			addr);
	}

	void initialize()
	{
		SetUnhandledExceptionFilter(exception_filter);

		const utils::nt::library ntdll("ntdll.dll");
		auto* set_filter = ntdll.get_proc<void(*)(LPTOP_LEVEL_EXCEPTION_FILTER)>("RtlSetUnhandledExceptionFilter");
		
		set_filter(exception_filter);
		utils::hook::jump(set_filter, set_unhandled_exception_filter_stub);

		utils::hook::jump(game::base_address + 0x2331560, write_mini_dump);
	}
}