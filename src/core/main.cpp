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

				const auto thread = [](LPVOID data) -> DWORD
				{
					static IDXGISwapChain* chain{ nullptr };
					
					while (!chain)
						chain = *reinterpret_cast<IDXGISwapChain**>(game::base_address + 0xF4B7858);

					std::this_thread::sleep_for(1s);

					rendering::initialize(chain);
					game::initialize();
					
					return 0;
				};

				CreateThread(nullptr, 0, thread, nullptr, 0, nullptr);
			}
			catch (const std::exception& ex) 
			{
				MessageBoxA(nullptr, ex.what(), "Exception Caught", 0);
			}
			catch (...) 
			{
				MessageBoxA(nullptr, "Something unexpected happened", "Exception Caught!", 0);
			}

			break;
		}
		
		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}

extern "C" __declspec(dllexport)
HRESULT D3D11CreateDevice(void* adapter, const uint64_t driver_type, const HMODULE software, const UINT flags, const void* p_feature_levels, const UINT feature_levels, const UINT sdk_version, void** device, void* feature_level, void** immediate_context)
{
	char path[MAX_PATH] = { 0 };
	GetSystemDirectoryA(path, sizeof path);

	const auto d3d11 = utils::nt::library::load(path + "/d3d11.dll"s);
	return d3d11.get_proc<decltype(&D3D11CreateDevice)>("D3D11CreateDevice")(adapter, driver_type, software, flags, p_feature_levels, feature_levels, sdk_version, device, feature_level, immediate_context);
}
