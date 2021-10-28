#include "dependencies/std_include.hpp"
#include "rendering.hpp"

namespace rendering
{
	utils::hook::detour present_hook;
	
	HRESULT __stdcall present(IDXGISwapChain* thisptr, UINT syncInterval, UINT flags)
	{
		menu::initialize(thisptr);
		dx::on_every_frame();

		return present_hook.invoke<HRESULT>(thisptr, syncInterval, flags);
	}
	
	void initialize()
	{
		present_hook.create((**reinterpret_cast<void****>(game::swap_chain))[8], present);
	}
}