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
	
	void initialize()
	{
		if (const auto chain = *reinterpret_cast<IDXGISwapChain**>(game::base_address + 0xF4B7858))
		{
			present_original = utils::hook::vtable(chain, 8, &present);
		}
	}
}