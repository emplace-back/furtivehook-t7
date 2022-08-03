#include "dependencies/std_include.hpp"
#include "security.hpp"

namespace security
{
	utils::hook::detour create_mutex_hook;
	utils::hook::detour nt_query_system_information_hook; 

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
			const auto window_text = utils::string::convert(wstr);

			const auto ignore =
			{
				"ida"s,
			};

			const auto result = std::any_of(ignore.begin(), ignore.end(), [=](const auto& string) { return utils::string::contains(utils::string::to_lower(window_text), string); });

			if (result)
			{
				return 0;
			}

			game::I_strncpyz(string, window_text, count);
		}

		return count;
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
		
		utils::hook::jump(GetWindowTextA, get_window_text, true, true);
		utils::hook::move_hook(GetWindowTextA);

		utils::hook::iat("kernel32.dll", "IsProcessorFeaturePresent", is_processor_feature_present); 
	}
}