#include "dependencies/std_include.hpp"
#include "security.hpp"

namespace security
{
	void* __stdcall virtual_alloc(void* address, size_t size, DWORD allocation_type, DWORD protect)
	{
		if (protect != PAGE_EXECUTE_READWRITE)
		{
			return VirtualAlloc(address, size, allocation_type, protect);
		}

		PRINT_LOG_DETAILED("Called at offset 0x%llX", uintptr_t(_ReturnAddress()) - game::base_address);
		std::abort();
	}

	bool __stdcall is_processor_feature_present(DWORD processor_feature)
	{
		const auto result = IsProcessorFeaturePresent(processor_feature);

		if (result)
		{
			PRINT_LOG_DETAILED("Called at offset 0x%llX", uintptr_t(_ReturnAddress()) - game::base_address);
			return 0;
		}
		
		return result;
	}
	
	void initialize()
	{
		utils::hook::iat("kernel32.dll", "VirtualAlloc", virtual_alloc);
		utils::hook::iat("kernel32.dll", "IsProcessorFeaturePresent", is_processor_feature_present);
	}
}