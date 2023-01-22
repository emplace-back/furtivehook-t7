#include "dependencies/std_include.hpp"
#include "security.hpp"

namespace security
{
	utils::hook::detour rb_draw_text_2d_cmd_hook;
	utils::hook::detour r_read_char_from_string_hook;
	utils::hook::detour ui_cod_read_char_from_string_hook; 
	utils::hook::detour lua_cmd_parse_args_hook;
	utils::hook::detour cl_rank_get_paragon_icon_name_hook;
	utils::hook::detour net_enqueue_packet_hook;
	utils::hook::detour cl_get_config_string_hook;
	utils::hook::detour bg_cache_get_event_string_name_for_index_hook;
	
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
		auto ctx = reinterpret_cast<PCONTEXT>(OFFSET(0x7FF6DFB86910));
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
			PRINT_LOG("Lua_CmdParseArgs called with the VM '%s' for function %s.\nText: %s\nArgs: %s", *argv, function, *textIn, command::args{ argsPriv }.join(0).data());
		}
		else
		{
			PRINT_LOG("Lua_CmdParseArgs called without a VM specified for function %s.\nText: %s\nArgs: %s", function, *textIn, command::args{ argsPriv }.join(0).data());
		}
#if 0
		return lua_cmd_parse_args_hook.call<void>(function, textIn, argsPriv, argc, argv);
#endif
	}

	const char* __fastcall cl_rank_get_paragon_icon_name(game::eModes mode, const int iconId)
	{
		return cl_rank_get_paragon_icon_name_hook.call<const char*>(mode, std::min(static_cast<uint32_t>(iconId), 63u));
	}
	
	bool __fastcall net_enqueue_packet(void* queue, uint32_t flags, game::netsrc_t sock, const game::netadr_t* addr, int length, const void* data)
	{
		const auto write_length = static_cast<uint32_t>(length + 3) + 48;

		if (sizeof(game::PacketQueueBlock::data) >= write_length)
		{
			return net_enqueue_packet_hook.call<bool>(queue, flags, sock, addr, length, data);
		}

		DEBUG_LOG("Exploit attempt caught! [%u] from %s", length, utils::get_sender_string(*addr).data());
	}

	const char* __fastcall cl_get_config_string(int config_string_index)
	{
		if (constexpr auto max_index = 3629u; static_cast<std::uint32_t>(config_string_index) <= max_index)
		{
			return cl_get_config_string_hook.call<const char*>(config_string_index);
		}

		PRINT_LOG("(configStringIndex) > (MAX_CONFIGSTRINGS) [%u] [0x%llX]",
			config_string_index,
			uintptr_t(_ReturnAddress()) - game::get_base());

		return "";
	}

	const char* __fastcall bg_cache_get_event_string_name_for_index(int inst, int index)
	{
		if (constexpr auto max_index = 0x100; static_cast<uint32_t>(index) <= max_index)
		{
			return bg_cache_get_event_string_name_for_index_hook.call<const char*>(inst, index);
		}

		PRINT_LOG("Crash attempt caught! [%u] [0x%llX]",
			index,
			uintptr_t(_ReturnAddress()) - game::get_base());

		return "";
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
			a.jmp(OFFSET(0x7FF6C72F9F45));

			a.bind(ret);
			a.jmp(OFFSET(0x7FF6C72F9F8B));
		});
		
		rb_draw_text_2d_cmd_hook.create(OFFSET(0x7FF6C6F3F5A0), rb_draw_text_2d_cmd);
		r_read_char_from_string_hook.create(OFFSET(0x7FF6C6F90F30), r_read_char_from_string);
		ui_cod_read_char_from_string_hook.create(OFFSET(0x7FF6C720E6C0), ui_cod_read_char_from_string);
		lua_cmd_parse_args_hook.create(OFFSET(0x7FF6C71E54E0), lua_cmd_parse_args);
		cl_rank_get_paragon_icon_name_hook.create(OFFSET(0x7FF6C66AAD20), cl_rank_get_paragon_icon_name);
		net_enqueue_packet_hook.create(OFFSET(0x7FF6C7458600), net_enqueue_packet);
		cl_get_config_string_hook.create(OFFSET(0x7FF6C6601110), cl_get_config_string);
		bg_cache_get_event_string_name_for_index_hook.create(OFFSET(0x7FF6C53878A0), bg_cache_get_event_string_name_for_index);

		utils::hook::jump(OFFSET(0x7FF6C72F9F40), ui_model_get_model_from_path_stub);
		utils::hook::jump(OFFSET(0x7FF6C73CF7B6), OFFSET(0x7FF6C73CF788)); // Cmd_TokenizeStringInternal
		
		utils::hook::iat("kernel32.dll", "IsProcessorFeaturePresent", is_processor_feature_present);
		utils::hook::iat("kernel32.dll", "UnhandledExceptionFilter", unhandled_exception_filter);
	}
}