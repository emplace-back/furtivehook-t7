#include "dependencies/std_include.hpp"
#include "arxan.hpp"

namespace arxan
{
	utils::hook::detour create_mutex_hook;

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
	
	void initialize()
	{
		create_mutex_hook.create(CreateMutexExA, create_mutex);
		
		utils::hook::jump(GetWindowTextA, get_window_text, true, true);
		utils::hook::move_hook(GetWindowTextA);
	}
}