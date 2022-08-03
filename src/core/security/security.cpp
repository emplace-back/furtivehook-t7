#include "dependencies/std_include.hpp"
#include "security.hpp"

namespace security
{
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
		utils::hook::iat("kernel32.dll", "IsProcessorFeaturePresent", is_processor_feature_present); 
	}
}