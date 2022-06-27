#include "dependencies/std_include.hpp"
#include "minidump.hpp"

namespace utils::exception::minidump
{
	constexpr MINIDUMP_TYPE get_minidump_type()
	{
		const auto type = MiniDumpIgnoreInaccessibleMemory
			| MiniDumpWithHandleData
			| MiniDumpScanMemory
			| MiniDumpWithProcessThreadData
			| MiniDumpWithFullMemoryInfo
			| MiniDumpWithThreadInfo
			| MiniDumpWithUnloadedModules;

		return static_cast<MINIDUMP_TYPE>(type);
	}

	bool write(const LPEXCEPTION_POINTERS ex)
	{
		const auto file_path = utils::string::get_log_file("minidumps", "dmp");
		if (!utils::io::write_file(file_path, "Creating minidump .."))
		{
			return false;
		}

		const auto file_handle = CreateFileA(file_path.data(), GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			nullptr);

		MINIDUMP_EXCEPTION_INFORMATION minidump_exception_info = { GetCurrentThreadId(), ex, FALSE };

		return MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle, get_minidump_type(), &minidump_exception_info, nullptr, nullptr);
	}
}