#include "dependencies/std_include.hpp"
#include "security.hpp"

namespace security
{
	utils::hook::detour lua_cmd_parse_args_hook;
	utils::hook::detour cl_rank_get_paragon_icon_name_hook;
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

	void __fastcall lua_cmd_parse_args(const char* function, const char** textIn, game::CmdArgs* argsPriv, int* argc, const char** argv)
	{
		if (*argc == 1)
		{
			PRINT_LOG("Lua_CmdParseArgs called with the VM %s for function %s. (text: %s | %s args: %s)", *argv, function, textIn, *textIn, command::args{}.join(0).data());
		}
		else
		{
			PRINT_LOG("Lua_CmdParseArgs called without a VM specified for function %s. (text: %s | %s args: %s)", function, textIn, *textIn, command::args{}.join(0).data());
		}
	}

	const char* __fastcall cl_rank_get_paragon_icon_name(game::eModes mode, const int iconId)
	{
		return cl_rank_get_paragon_icon_name_hook.call<const char*>(mode, std::min(static_cast<uint32_t>(iconId), 64u));
	}
	
	void initialize()
	{
		const auto ui_model_get_model_from_path_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			const auto ret = a.newLabel();

			a.cmp(rax, 64); 
			a.jae(ret);

			a.movzx(ecx, byte_ptr(rbx));
			a.test(cl, cl);
			a.jmp(game::base_address + 0x2018F45);

			a.bind(ret);
			a.jmp(game::base_address + 0x2018F8B);
		}); 
		
		rb_draw_text_2d_cmd_hook.create(game::base_address + 0x1C5E5A0, rb_draw_text_2d_cmd);
		r_read_char_from_string_hook.create(game::base_address + 0x1CAFF30, r_read_char_from_string);
		ui_cod_read_char_from_string_hook.create(game::base_address + 0x1F2D6C0, ui_cod_read_char_from_string);
		lua_cmd_parse_args_hook.create(game::base_address + 0x1F044E0, lua_cmd_parse_args);
		cl_rank_get_paragon_icon_name_hook.create(game::base_address + 0x13C9D20, cl_rank_get_paragon_icon_name);

		utils::hook::jump(game::base_address + 0x2018F40, ui_model_get_model_from_path_stub);
		utils::hook::jump(game::base_address + 0x20EE7B6, game::base_address + 0x20EE788); // Cmd_TokenizeStringInternal
		
		utils::hook::iat("kernel32.dll", "IsProcessorFeaturePresent", is_processor_feature_present);
		utils::hook::iat("kernel32.dll", "UnhandledExceptionFilter", unhandled_exception_filter);
	}
}