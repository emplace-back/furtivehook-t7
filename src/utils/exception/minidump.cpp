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

	std::string get_temp_filename()
	{
		char filename[MAX_PATH] = { 0 };
		char pathname[MAX_PATH] = { 0 };

		GetTempPathA(sizeof(pathname), pathname);
		GetTempFileNameA(pathname, "t7-", 0, filename);
		return filename;
	}

	HANDLE write_dump_to_temp_file(const LPEXCEPTION_POINTERS ex)
	{
		MINIDUMP_EXCEPTION_INFORMATION minidump_exception_info = { GetCurrentThreadId(), ex, FALSE };

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
			utils::nt::terminate();
		}

		return file_handle;
	}

	std::string read_file(HANDLE file_handle)
	{
		FlushFileBuffers(file_handle);
		SetFilePointer(file_handle, 0, nullptr, FILE_BEGIN);

		std::string buffer{};

		DWORD bytes_read = 0;
		char temp_bytes[0x2000] = { 0 };

		do
		{
			if (ReadFile(file_handle, temp_bytes, sizeof(temp_bytes), &bytes_read, nullptr))
			{
				buffer.append(temp_bytes, bytes_read); 
			}
		} while (bytes_read == sizeof(temp_bytes));

		CloseHandle(file_handle); 
		return buffer;
	}

	std::string create_minidump(const LPEXCEPTION_POINTERS ex)
	{
		auto* const file_handle = write_dump_to_temp_file(ex);
		return read_file(file_handle);
	}

	void write_minidump(const LPEXCEPTION_POINTERS ex)
	{
		const auto filename = utils::generate_log_filename("furtivehook\\logs\\minidumps\\", "dmp");
		const auto data = create_minidump(ex);
		utils::io::write_file(filename, data);
	}
}