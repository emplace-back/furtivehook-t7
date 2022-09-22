#include "dependencies/std_include.hpp"
#include "security.hpp"

namespace security
{
	utils::hook::detour rb_draw_text_2d_cmd_hook; 
	utils::hook::detour r_read_char_from_string_hook;
	utils::hook::detour ui_cod_read_char_from_string_hook;
	
	BOOL __stdcall is_processor_feature_present(DWORD feature)
	{
		if (feature == PF_FASTFAIL_AVAILABLE)
		{
			return FALSE;
		}

		return IsProcessorFeaturePresent(feature);
	}

	LONG __stdcall unhandled_exception_filter(PEXCEPTION_POINTERS ex)
	{
		auto ctx = reinterpret_cast<PCONTEXT>(game::base_address + 0x1A8A5910);
		RaiseException(STATUS_STACK_BUFFER_OVERRUN, 0, 1, reinterpret_cast<ULONG_PTR*>(&ctx));
		return FALSE;
	}
	
	void __fastcall rb_draw_text_2d_cmd(void* state, game::GfxRenderCommandExecState* execState)
	{
		utils::string::strip_materials(static_cast<char*>(execState->cmd) + 0x74);
		return rb_draw_text_2d_cmd_hook.call<void>(state, execState);
	}
	
	bool __fastcall ui_cod_read_char_from_string(LocalClientNum_t localClientNum, char** textPtr, unsigned int* letter, const float originalColor[4], void* color, char** buttonName, void** iconImage, unsigned int* cursor)
	{
		utils::string::strip_materials(*textPtr); 
		return ui_cod_read_char_from_string_hook.call<bool>(localClientNum, textPtr, letter, originalColor, color, buttonName, iconImage, cursor);
	}

	bool __fastcall r_read_char_from_string(char** textPtr, unsigned int* letter, char** buttonName, void* iconMaterial, unsigned int* cursor, int* colorIndex)
	{
		utils::string::strip_materials(*textPtr);
		return r_read_char_from_string_hook.call<bool>(textPtr, letter, buttonName, iconMaterial, cursor, colorIndex);
	}
	
	void initialize()
	{
		rb_draw_text_2d_cmd_hook.create(game::base_address + 0x1C5E5A0, rb_draw_text_2d_cmd);
		r_read_char_from_string_hook.create(game::base_address + 0x1CAFF30, r_read_char_from_string);
		ui_cod_read_char_from_string_hook.create(game::base_address + 0x1F2D6C0, ui_cod_read_char_from_string);

		utils::hook::jump(game::base_address + 0x2018F40, utils::hook::assemble([](utils::hook::assembler& a)
		{
			const auto ret = a.newLabel();

			a.cmp(rax, 64); // keySize >= MAX_UI_MODEL_KEY_CHARS
			a.jae(ret);

			a.movzx(ecx, byte_ptr(rbx));
			a.test(cl, cl);
			a.jmp(game::base_address + 0x2018F45);

			a.bind(ret);
			a.jmp(game::base_address + 0x2018F8B);
		}));
		
		utils::hook::iat("kernel32.dll", "IsProcessorFeaturePresent", is_processor_feature_present);
		utils::hook::iat("kernel32.dll", "UnhandledExceptionFilter", unhandled_exception_filter);
	}
}