#include "dependencies/std_include.hpp"
#include "rendering.hpp"

namespace rendering
{
	auto present_original = reinterpret_cast<decltype(&present)>(0);
	
	HRESULT __stdcall present(IDXGISwapChain* thisptr, UINT syncInterval, UINT flags)
	{
		menu::initialize(thisptr);
		dx::on_every_frame();

		scheduler::execute(scheduler::pipeline::backend);

		return present_original(thisptr, syncInterval, flags);
	}
	
	void initialize(const IDXGISwapChain* chain)
	{
		if (chain)
		{
			present_original = utils::hook::vtable(chain, 8, &present);
		}
	}
}