#include "dependencies/std_include.hpp"
#include "arxan.hpp"

namespace arxan
{
	utils::hook::detour get_thread_context_hook; 
	utils::hook::detour create_mutex_hook;
	utils::hook::detour nt_query_system_information_hook;
	utils::hook::detour nt_query_information_process_hook;
	
	bool remove_keyword_from_string(const UNICODE_STRING& string)
	{
		if (!string.Buffer || !string.Length)
		{
			return false;
		}

		const auto keywords =
		{
			L"IDA"s,
			L"ida"s,
		};

		const auto path = std::wstring_view{ string.Buffer, string.Length / sizeof(*string.Buffer) };

		auto modified{ false };

		for (const auto& keyword : keywords)
		{
			while (true)
			{
				const auto pos = path.find(keyword);
				if (pos == std::wstring::npos)
				{
					break;
				}

				modified = true;

				for (size_t i = 0; i < keyword.size(); ++i)
				{
					string.Buffer[pos + i] = L'a';
				}
			}
		}

		return modified;
	}

	bool remove_keyword_from_string(wchar_t* str, const size_t length)
	{
		UNICODE_STRING unicode_string{};
		unicode_string.Buffer = str;
		unicode_string.Length = static_cast<uint16_t>(length);
		unicode_string.MaximumLength = unicode_string.Length;

		return remove_keyword_from_string(unicode_string);
	}

	BOOL WINAPI get_thread_context_stub(const HANDLE thread_handle, const LPCONTEXT context)
	{
		constexpr auto debug_registers_flag = (CONTEXT_DEBUG_REGISTERS & ~CONTEXT_AMD64);
		if (context->ContextFlags & debug_registers_flag)
		{
			auto* source = _ReturnAddress();
			const auto game = utils::nt::library{};
			const auto source_module = utils::nt::library::get_by_address(source);

			if (source_module == game)
			{
				context->ContextFlags &= ~debug_registers_flag;
			}
		}

		return get_thread_context_hook.call<BOOL>(thread_handle, context);
	}
	
	HANDLE __stdcall create_mutex(LPSECURITY_ATTRIBUTES attributes, const char* name, DWORD flags, DWORD access)
	{
		if (name == "$ IDA trusted_idbs"s || name == "$ IDA registry mutex $"s)
		{
			return nullptr;
		}

		return create_mutex_hook.call<HANDLE>(attributes, name, flags, access);
	}

	int __stdcall get_window_text(HWND window, char* string, size_t max_count)
	{
		std::wstring wstr{};
		wstr.resize(max_count);

		const auto count = GetWindowTextW(window, &wstr[0], max_count);

		if (count)
		{
			remove_keyword_from_string(wstr.data(), count);

			const std::string regular_str(wstr.begin(), wstr.end());
			memset(string, 0, max_count);
			memcpy(string, regular_str.data(), count);
		}

		return count;
	}

	NTSTATUS __stdcall nt_query_information_process_stub(HANDLE handle, PROCESSINFOCLASS info_class, void* info, uint32_t info_length, uint32_t* return_length)
	{
		const auto status = nt_query_information_process_hook.call<NTSTATUS>(handle, info_class, info, info_length, return_length);

		if (NT_SUCCESS(status))
		{
			if (info_class == ProcessImageFileName || static_cast<int>(info_class) == 43)
			{
				remove_keyword_from_string(*static_cast<UNICODE_STRING*>(info));
			}
		}

		return status;
	}
	
	NTSTATUS __stdcall nt_query_system_information(SYSTEM_INFORMATION_CLASS system_information_class, void* system_information, uint32_t system_information_length, uint32_t* return_length)
	{
		const auto status = nt_query_system_information_hook.call<NTSTATUS>(system_information_class, system_information, system_information_length, return_length);

		if (NT_SUCCESS(status))
		{
			if (system_information_class == SystemProcessInformation)
			{
				auto addr = static_cast<uint8_t*>(system_information);
				
				while (true)
				{
					const auto info = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(addr);
					remove_keyword_from_string(info->ImageName);

					if (!info->NextEntryOffset)
					{
						break;
					}

					addr += info->NextEntryOffset;
				}
			}
		}

		return status;
	}

	struct integrity_handler_context
	{
		uint32_t* computed_checksum;
		uint32_t* original_checksum;
	};

	bool is_handler_context(uint8_t* stack_frame, const uint32_t computed_checksum, const uint32_t frame_offset)
	{
		auto* potential_address = *reinterpret_cast<uint32_t**>(stack_frame + frame_offset);

		int64_t diff = reinterpret_cast<uint64_t>(stack_frame) - reinterpret_cast<uint64_t>(potential_address);
		diff = std::abs(diff);

		return diff < 0x1000 && *potential_address == computed_checksum;
	}

	integrity_handler_context* search_handler_context(uint8_t* stack_frame, const uint32_t computed_checksum)
	{
		for (uint32_t frame_offset = 0x38; frame_offset < 0x90; frame_offset += 8)
		{
			if (is_handler_context(stack_frame, computed_checksum, frame_offset))
			{
				return reinterpret_cast<integrity_handler_context*>(stack_frame + frame_offset);
			}
		}

		return nullptr;
	}

	uint32_t adjust_integrity_checksum(const uint64_t return_address, uint8_t* stack_frame, const uint32_t current_checksum)
	{
		const auto* context = search_handler_context(stack_frame, current_checksum);

		if (!context)
		{
			PRINT_LOG_DETAILED("Unable to find frame offset for: %llX", return_address);
			return current_checksum;
		}

		const auto correct_checksum = *context->original_checksum;
		*context->computed_checksum = correct_checksum;
		return correct_checksum;
	}

	void patch_intact_basic_block_integrity_check(void* address)
	{
		const auto game_address = reinterpret_cast<uint64_t>(address);
		constexpr auto inst_len = 3;

		const auto next_inst_addr = game_address + inst_len;
		const auto next_inst = *reinterpret_cast<uint32_t*>(next_inst_addr);

		if ((next_inst & 0xFF00FFFF) != 0xFF004583)
		{
			throw std::runtime_error(utils::string::va("Unable to patch intact basic block: %llX", game_address));
		}

		const auto other_frame_offset = static_cast<uint8_t>(next_inst >> 16);
		static const auto stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(rax);

			a.mov(rax, qword_ptr(rsp, 8));
			a.sub(rax, 2); // Skip the push we inserted

			a.push(rax);
			a.pushad64();

			a.mov(r8, qword_ptr(rsp, 0x88));
			a.mov(rcx, rax);
			a.mov(rdx, rbp);
			a.call_aligned(adjust_integrity_checksum);

			a.mov(qword_ptr(rsp, 0x80), rax);

			a.popad64();
			a.pop(rax);

			a.add(rsp, 8);

			a.mov(dword_ptr(rdx, rcx, 4), eax);

			a.pop(rax); // return addr
			a.xchg(rax, qword_ptr(rsp)); // switch with push

			a.add(dword_ptr(rbp, rax), 0xFFFFFFFF);

			a.mov(rax, dword_ptr(rdx, rcx, 4)); // restore rax

			a.ret();
		});

		// push other_frame_offset
		utils::hook::set<uint16_t>(game_address, static_cast<uint16_t>(0x6A | (other_frame_offset << 8)));
		utils::hook::call(game_address + 2, stub);
	}

	void patch_split_basic_block_integrity_check(void* address)
	{
		const auto game_address = reinterpret_cast<uint64_t>(address);
		constexpr auto inst_len = 3;

		const auto next_inst_addr = game_address + inst_len;

		if (*reinterpret_cast<uint8_t*>(next_inst_addr) != 0xE9)
		{
			throw std::runtime_error(utils::string::va("Unable to patch split basic block: %llX", game_address));
		}

		const auto jump_target = utils::hook::extract<void*>(reinterpret_cast<void*>(next_inst_addr + 1));
		const auto stub = utils::hook::assemble([jump_target](utils::hook::assembler& a)
		{
			a.push(rax);

			a.mov(rax, qword_ptr(rsp, 8));
			a.push(rax);

			a.pushad64();

			a.mov(r8, qword_ptr(rsp, 0x88));
			a.mov(rcx, rax);
			a.mov(rdx, rbp);
			a.call_aligned(adjust_integrity_checksum);

			a.mov(qword_ptr(rsp, 0x80), rax);

			a.popad64();
			a.pop(rax);

			a.add(rsp, 8);

			a.mov(dword_ptr(rdx, rcx, 4), eax);

			a.add(rsp, 8);

			a.jmp(jump_target);
		});

		utils::hook::call(game_address, stub);
	}

	void search_and_patch_integrity_checks()
	{
		const auto intact_results = "89 04 8A 83 45 ? FF"_sig;
		const auto split_results = "89 04 8A E9"_sig;

		for (auto* i : intact_results)
		{
			patch_intact_basic_block_integrity_check(i);
		}

		for (auto* i : split_results)
		{
			patch_split_basic_block_integrity_check(i);
		}
	}

	void initialize()
	{
		create_mutex_hook.create(CreateMutexExA, create_mutex);

		const utils::nt::library ntdll("ntdll.dll"); 
		nt_query_information_process_hook.create(ntdll.get_proc<void*>("NtQueryInformationProcess"), nt_query_information_process_stub);
		nt_query_system_information_hook.create(ntdll.get_proc<void*>("NtQuerySystemInformation"), nt_query_system_information);
		nt_query_system_information_hook.move();

		auto* get_thread_context_func = utils::nt::library("kernelbase.dll").get_proc<void*>("GetThreadContext");
		get_thread_context_hook.create(get_thread_context_func, get_thread_context_stub);
		
		utils::hook::jump(GetWindowTextA, get_window_text, true, true);
		utils::hook::move_hook(GetWindowTextA);

		scheduler::once(search_and_patch_integrity_checks, scheduler::pipeline::main);
	}

	void uninitialize()
	{
		nt_query_system_information_hook.clear();
	}
}