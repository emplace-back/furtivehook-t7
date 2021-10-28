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

	std::string generate_minidump_filename()
	{
		return utils::generate_log_filename("furtivehook\\logs\\minidumps\\", "dmp");
	}

	std::string get_temp_filename()
	{
		char filename[MAX_PATH] = { 0 };
		char pathname[MAX_PATH] = { 0 };

		GetTempPathA(sizeof(pathname), pathname);
		GetTempFileNameA(pathname, "t7-", 0, filename);
		return filename;
	}

	HANDLE write_dump_to_temp_file(const LPEXCEPTION_POINTERS exceptioninfo)
	{
		MINIDUMP_EXCEPTION_INFORMATION minidump_exception_info = { GetCurrentThreadId(), exceptioninfo, FALSE };

		auto* const file_handle = CreateFileA(get_temp_filename().data(), GENERIC_WRITE | GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
			FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE,
			nullptr);

		if (!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file_handle, get_minidump_type(),
			&minidump_exception_info,
			nullptr,
			nullptr))
		{
			PRINT_LOG("There was an error creating the minidump.");
			utils::nt::terminate(123);
		}

		return file_handle;
	}

	std::string read_file(HANDLE file_handle)
	{
		FlushFileBuffers(file_handle);
		SetFilePointer(file_handle, 0, nullptr, FILE_BEGIN);

		std::string buffer{};

		DWORD bytes_read = 0;
		char temp_bytes[0x2000];

		do
		{
			if (!ReadFile(file_handle, temp_bytes, sizeof(temp_bytes), &bytes_read, nullptr))
			{
				CloseHandle(file_handle); 
				return {};
			}

			buffer.append(temp_bytes, bytes_read);
		} while (bytes_read == sizeof(temp_bytes));

		CloseHandle(file_handle); 
		return buffer;
	}

	std::string create_minidump(const LPEXCEPTION_POINTERS exceptioninfo)
	{
		auto* const file_handle = write_dump_to_temp_file(exceptioninfo);
		return read_file(file_handle);
	}
}