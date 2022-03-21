#include "dependencies/std_include.hpp"
#include "dx.hpp"
#include <d3d11.h>

namespace rendering::dx
{
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr; 
	
	void on_every_frame()
	{
		if (menu::initialized)
		{
			ImGui::GetIO().MouseDrawCursor = menu::is_open();
			
			ImGui_ImplWin32_NewFrame();
			ImGui_ImplDX11_NewFrame();
			ImGui::NewFrame();

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(0, 100, 200, 255).Value);
			utils::toast::draw_toast();
			ImGui::PopStyleColor();

			esp::draw();
			menu::draw();

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
	}
	
	void on_frame(IDXGISwapChain* swap_chain)
	{
		if (const auto window = game::window; window)
		{
			input::initialize(window);

			swap_chain->GetDevice(__uuidof(device), reinterpret_cast<void**>(&device));
			device->GetImmediateContext(&context);

			ImGui::CreateContext();
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(device, context);

			menu::set_style();
		}
	}
}