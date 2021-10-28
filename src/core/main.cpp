#include "dependencies/std_include.hpp"

BOOL __stdcall DllMain(const HMODULE module, const DWORD reason, const LPVOID /*reserved*/)
{
	switch (reason) 
	{
		case DLL_PROCESS_ATTACH:
		{
			try 
			{
				DisableThreadLibraryCalls(module);

				game::initialize();
			}
			catch (const std::exception& ex) 
			{
				MessageBoxA(nullptr, ex.what(), "Exception Caught", 0);
			}
			catch (...) 
			{
				MessageBoxA(nullptr, "Something unexpected happened", "Exception Caught", 0);
			}

			break;
		}
		
		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}
