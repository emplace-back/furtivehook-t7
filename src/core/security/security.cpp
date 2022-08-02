#include "dependencies/std_include.hpp"
#include "security.hpp"

namespace security
{
	utils::hook::detour create_mutex_hook;
	utils::hook::detour nt_query_system_information_hook; 
	
	bool is_in_list(const std::string& path)
	{
		const auto ignore =
		{
			"ida"s,
		};

		return std::any_of(ignore.begin(), ignore.end(), [=](const auto& string) { return utils::string::contains(utils::string::to_lower(path), string); });
	}

	bool is_in_list(const UNICODE_STRING& string)
	{
		if (!string.Buffer || !string.Length)
		{
			return false;
		}

		const auto path = std::wstring_view(string.Buffer, string.Length / sizeof(string.Buffer[0]));
		const auto path_str = std::string{ path.begin(), path.end() };
		
		return security::is_in_list(path_str);
	}

	HANDLE __stdcall create_mutex(LPSECURITY_ATTRIBUTES attributes, const char* name, DWORD flags, DWORD access)
	{
		if (name == "$ IDA trusted_idbs"s || name == "$ IDA registry mutex $"s)
		{
			return nullptr;
		}

		return create_mutex_hook.invoke<HANDLE>(attributes, name, flags, access);
	}

	int __stdcall get_window_text(HWND window, char* string, size_t max_count)
	{
		std::wstring wstr{};
		wstr.resize(max_count);
		
		const auto count = GetWindowTextW(window, &wstr[0], max_count);
		
		if (count)
		{
			const auto path = utils::string::convert(wstr);
			const auto is_invalid = security::is_in_list(path);

			if (is_invalid)
			{
				return 0;
			}

			utils::hook::write_string(string, path);
		}

		return count;
	}
	
	NTSTATUS __stdcall nt_query_system_information(SYSTEM_INFORMATION_CLASS system_information_class, void* system_information, size_t system_information_length, size_t* return_length)
	{
		const auto status = nt_query_system_information_hook.invoke<NTSTATUS>(system_information_class, system_information, system_information_length, return_length);

		if (NT_SUCCESS(status))
		{
			if (system_information_class == SystemProcessInformation)
			{
				auto addr = static_cast<uint8_t*>(system_information);
				
				while (true)
				{
					const auto info = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(addr); 
					const auto is_invalid = security::is_in_list(info->ImageName);

					if (!info->NextEntryOffset || is_invalid)
					{
						break;
					}

					addr += info->NextEntryOffset;
				}
			}
		}

		return status;
	}
	
	BOOL __stdcall is_processor_feature_present(DWORD processor_feature)
	{
		const auto result = IsProcessorFeaturePresent(processor_feature);

		if (result)
		{
			PRINT_LOG_DETAILED("Called at offset 0x%llX", reinterpret_cast<uintptr_t>(_ReturnAddress()) - game::base_address);
		}

		return result;
	}
	
	void initialize()
	{
		create_mutex_hook.create(CreateMutexExA, create_mutex);

		utils::hook::jump(GetWindowTextA, get_window_text, true, true, true);

		utils::hook::iat("kernel32.dll", "IsProcessorFeaturePresent", is_processor_feature_present); 
		
		const utils::nt::library ntdll("ntdll.dll");
		nt_query_system_information_hook.create(ntdll.get_proc<void*>("NtQuerySystemInformation"), nt_query_system_information, true);
	}
}