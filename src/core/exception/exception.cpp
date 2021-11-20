#include "dependencies/std_include.hpp"
#include "exception.hpp"

namespace exception
{
	bool is_harmless_error(const std::uint32_t code)
	{
		return code == STATUS_INTEGER_OVERFLOW
			|| code == STATUS_FLOAT_OVERFLOW;
	}
	
	LONG __stdcall exception_filter(const LPEXCEPTION_POINTERS ex)
	{
		const auto addr = ex->ExceptionRecord->ExceptionAddress;
		const auto code = ex->ExceptionRecord->ExceptionCode;

		if (is_harmless_error(code))
		{
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		
		ex->ContextRecord->Dr0 = game::base_address + 0x134BDAD;
		ex->ContextRecord->Dr1 = game::base_address + 0x1EF7094;
		ex->ContextRecord->Dr2 = game::base_address + 0x1439606;
		ex->ContextRecord->Dr3 = game::base_address + 0x1CD8B43;
		ex->ContextRecord->Dr7 = (1 << 0) | (1 << 2) | (1 << 4) | (1 << 6); 

		if (ex->ContextRecord->Rip == game::base_address + 0x9C2AF0)
		{
			events::cg_predict_playerstate();
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		
		if (ex->ContextRecord->Rip == game::base_address + 0x2522A12)
		{
			PRINT_LOG("[DDL_CreateContext] Exploit attempt caught!");
			ex->ContextRecord->Rip = game::base_address + 0x2522ABD;
			return EXCEPTION_CONTINUE_EXECUTION;
		}

		if (ex->ContextRecord->Rip == game::base_address + 0x2522B4A)
		{
			PRINT_LOG("[DDL_ResetContext] Exploit attempt caught!");
			ex->ContextRecord->Rip = game::base_address + 0x2522B85;
			return EXCEPTION_CONTINUE_EXECUTION;
		}

		const auto index = static_cast<dvar::exception_index>(ex->ContextRecord->Rcx);

		if (const auto exception_func = dvar::exceptions.find(index); exception_func != dvar::exceptions.end())
		{
			exception_func->second(ex);
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		
		if (const auto exception_func = hbp::exceptions.find(ex->ContextRecord->Rip); exception_func != hbp::exceptions.end())
		{
			return exception_func->second(ex);
		}

		auto error = "Termination due to a stack overflow."s;
		if (code != EXCEPTION_STACK_OVERFLOW)
		{
			error = utils::string::va("Exception: 0x%08X at offset 0x%llX", code, reinterpret_cast<std::uintptr_t>(addr) - game::base_address);
		}

		const auto filename = utils::exception::minidump::generate_minidump_filename();
		utils::io::write_minidump(ex, filename, error);
		game::Com_Error(nullptr, 0, game::ERROR_DROP, error.data());
		
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	LPTOP_LEVEL_EXCEPTION_FILTER __stdcall set_unhandled_exception_filter(LPTOP_LEVEL_EXCEPTION_FILTER)
	{
		return &exception_filter;
	}

	void initialize()
	{
		SetUnhandledExceptionFilter(exception_filter);
		utils::hook::jump(&SetUnhandledExceptionFilter, &set_unhandled_exception_filter);

		utils::hook::set<std::uintptr_t>(game::base_address + 0x168EEAC0, dvar::main);
		utils::hook::set<std::uintptr_t>(game::base_address + 0xAE96C40, dvar::renderer);
		utils::hook::set<std::uintptr_t>(game::base_address + 0x53DD160, dvar::predict_ps);

		exception::dvar::register_exception(exception::dvar::predict_ps, [](auto& ex)
		{
			ex->ContextRecord->Rcx = reinterpret_cast<std::uintptr_t>(game::unknown_original);
			ex->ContextRecord->Rbx = reinterpret_cast<std::uintptr_t>(game::unknown_original);
			ex->ContextRecord->Rip = game::base_address + 0x22BC95E;

			DWORD dwProtection = PAGE_EXECUTE | PAGE_GUARD;
			VirtualProtect(reinterpret_cast<void*>(game::base_address + 0x9C2AF0), sizeof(BYTE), dwProtection, &dwProtection);
		});
		
		hbp::register_exception(game::base_address + 0x1439606, [](const auto& ex)
		{
			ex->ContextRecord->Rsp -= 0x60;

			const auto sender_id = ex->ContextRecord->Rcx;
			const auto message = reinterpret_cast<const char*>(ex->ContextRecord->R8);
			const auto message_size = ex->ContextRecord->R9;

			if (events::instant_message::dispatch::handle_message(sender_id, message, message_size))
			{
				ex->ContextRecord->Rip = game::base_address + 0x1439716;
				return EXCEPTION_CONTINUE_EXECUTION;
			}

			ex->ContextRecord->Rip += 4;
			return EXCEPTION_CONTINUE_EXECUTION;
		});

		hbp::register_exception(game::base_address + 0x1CD8B43, [](const auto& ex)
		{
			ex->ContextRecord->R8 = ex->ContextRecord->Rbx;
			ex->ContextRecord->Rip += 3;

			if (const auto& string = reinterpret_cast<char*>(ex->ContextRecord->Rsi); *string)
			{
				static auto replaced = ""s;
				replaced = std::regex_replace(std::string(string), std::regex("\\^[^0-9]"), "");
				utils::hook::write_string(string, replaced.data());
			}
			
			return EXCEPTION_CONTINUE_EXECUTION;
		});
	}
}