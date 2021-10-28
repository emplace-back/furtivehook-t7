#include "iat.hpp"

namespace security::iat
{
	void* __stdcall virtual_alloc(const LPVOID address, const SIZE_T size, const DWORD allocation_type, const DWORD protect)
	{
		if (protect != PAGE_EXECUTE_READWRITE)
		{
			return VirtualAlloc(address, size, allocation_type, protect);
		}

		PRINT_LOG_DETAILED("Called at (0x%08X)", _ReturnAddress());
		utils::nt::terminate();
	}

	void initialize()
	{
		loader::on_import("KERNEL32.dll", [](const auto& function) -> void*
		{		
			if (function == "VirtualAlloc")
			{
				return virtual_alloc;
			}

			return nullptr;
		});

		loader::load_library("BlackOps3.exe");
	}
}