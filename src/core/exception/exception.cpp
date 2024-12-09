#include "dependencies/std_include.hpp"
#include "exception.hpp"

namespace exception
{
	namespace
	{
		std::mutex exception_mutex; 
		
		bool handle(const LPEXCEPTION_POINTERS ex)
		{
			auto& ctx = *ex->ContextRecord; 
			const auto code = ex->ExceptionRecord->ExceptionCode;
			const auto addr = ctx.Rip;
			const auto base = game::get_base();
			const auto offset = addr - base;

			if (code < STATUS_ACCESS_VIOLATION)
				return false;

			const std::lock_guard<std::mutex> _(exception_mutex);

			const auto game = utils::nt::library{};
			const auto source_module = utils::nt::library::get_by_address(addr);

			const auto module_name = !source_module.get_name().empty() ? source_module.get_name() : "N/A";

			std::vector<uint8_t*> callstack;

			for (size_t i = 0; i < 0x80 && callstack.size() < 0x20; ++i)
			{
				const auto addr = *reinterpret_cast<uint8_t**>(ctx.Rsp + sizeof(uint64_t) + i);

				if (utils::nt::library::get_by_address(addr))
					callstack.push_back(addr);
			}

			auto message = utils::string::va("Exception: 0x%08X at 0x%llX", code, source_module == game ? game::derelocate(addr) : addr);

			if (source_module)
				message += utils::string::va("\n%s: 0x%llX", module_name.data(), source_module.get_ptr());

			if (!callstack.empty())
			{
				message += "\n\nStack trace:\n";

				for (const auto& stack : callstack)
				{
					const auto stack_module = utils::nt::library::get_by_address(stack);
					const auto stack_addr = stack_module == game ? game::derelocate(stack) : uintptr_t(stack);

					message += utils::string::va("0x%llX", stack_addr);

					if (const auto module_name = stack_module.get_name(); !module_name.empty())
						message += " (" + module_name + ")";

					message += "\n";
				}

				message.resize(message.size() - 1);
			}

			message += "\n\nRegisters:\n";

			const static std::vector<const char*> registers =
			{
				"rax",
				"rcx",
				"rdx",
				"rbx",
				"rsp",
				"rbp",
				"rsi",
				"rdi",
				"r8",
				"r9",
				"r10",
				"r11",
				"r12",
				"r13",
				"r14",
				"r15",
			};

			for (size_t i = 0; i < registers.size(); ++i)
			{
				auto value = *(&ctx.Rax + i);

				if (utils::nt::library::get_by_address(value) == game)
					value = game::derelocate(value);

				message += utils::string::va("%s: 0x%llX\n", registers[i], value);
			}

			message.resize(message.size() - 1);

			PRINT_LOG("%s", message.data());
			MessageBoxA(nullptr, message.data(), "Exception", MB_ICONERROR);

			if (!utils::exception::minidump::write(ex))
			{
				MessageBoxA(nullptr, utils::string::va("There was an error creating the minidump! (0x%08X)", GetLastError()).data(), "Minidump Error", MB_OK | MB_ICONERROR);
			}

			utils::nt::terminate(code);

			return true;
		}
		
		LONG __stdcall exception_filter(const LPEXCEPTION_POINTERS ex)
		{
			if (exception::handle(ex))
			{
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
		DEBUG_LOG("Exception handler called.");
	}

	void initialize()
	{
		const utils::nt::library ntdll("ntdll.dll");
		
		auto* set_filter = ntdll.get_proc<void(*)(LPTOP_LEVEL_EXCEPTION_FILTER)>("RtlSetUnhandledExceptionFilter");
		set_filter(exception_filter);
		utils::hook::jump(set_filter, set_unhandled_exception_filter_stub);

		SetUnhandledExceptionFilter(exception_filter);

		utils::hook::jump(OFFSET(0x7FF6C7612560), write_mini_dump);
	}
}