#include "dependencies/std_include.hpp"
#include "security.hpp"

namespace security
{
	utils::hook::detour handle_lobby_state_core_hook; 
	utils::hook::detour rb_draw_text_2d_cmd_hook;
	utils::hook::detour r_read_char_from_string_hook;
	utils::hook::detour ui_cod_read_char_from_string_hook; 
	utils::hook::detour lua_cmd_parse_args_hook;
	utils::hook::detour cl_rank_get_paragon_icon_name_hook;
	utils::hook::detour net_enqueue_packet_hook;
	utils::hook::detour cl_get_config_string_hook;
	utils::hook::detour bg_cache_get_event_string_name_for_index_hook;
	utils::hook::detour xanims_get_notetrack_time_hook;
	utils::hook::detour dobj_get_bone_index_hook;
	utils::hook::detour sv_packet_event_hook;
	utils::hook::detour sv_update_server_commands_to_client_hook;
	utils::hook::detour cl_parse_gamestate_hook;
	utils::hook::detour netchan_send_message_hook;
	
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
		utils::string::clean_ui_text(static_cast<char*>(execState->cmd) + 0x74);
		return rb_draw_text_2d_cmd_hook.call<void>(state, execState);
	}
	
	bool __fastcall ui_cod_read_char_from_string(LocalClientNum_t localClientNum, char** textPtr, unsigned int* letter, const float originalColor[4], void* color, char** buttonName, void** iconImage, unsigned int* cursor)
	{
		utils::string::clean_ui_text(*textPtr);
		return ui_cod_read_char_from_string_hook.call<bool>(localClientNum, textPtr, letter, originalColor, color, buttonName, iconImage, cursor);
	}

	bool __fastcall r_read_char_from_string(char** textPtr, unsigned int* letter, char** buttonName, void* iconMaterial, unsigned int* cursor, int* colorIndex)
	{
		utils::string::clean_ui_text(*textPtr);
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

	utils::hook::detour lobby_msg_rw_package_u_char_hook;

	bool __fastcall lobby_msg_rw_package_u_char(game::msg_t* msg, const char* key, uint8_t* value)
	{
		if (msg->packageType == game::PACKAGE_TYPE_WRITE && key == "paragonIconId"s)
		{
			DEBUG_LOG("%i", *value);
			*value = 69;
		}
	
		return lobby_msg_rw_package_u_char_hook.call<bool>(msg, key, value);
	}

	const char* __fastcall cl_rank_get_paragon_icon_name(game::eModes mode, const int iconId)
	{
		constexpr auto max_index = 64; 
		const auto index = std::min(iconId, max_index - 1);

		return cl_rank_get_paragon_icon_name_hook.call<const char*>(mode, index);
	}
	
	bool __fastcall net_enqueue_packet(void* queue, uint32_t flags, game::netsrc_t sock, const game::netadr_t* addr, int length, const void* data)
	{
		const auto write_length = static_cast<uint32_t>(length + 3) + 48;

		if (sizeof(game::PacketQueueBlock::data) >= write_length)
		{
			return net_enqueue_packet_hook.call<bool>(queue, flags, sock, addr, length, data);
		}

		DEBUG_LOG("Exploit attempt caught! [%u] from %s", length, utils::get_sender_string(*addr).data());
		return false;
	}

	const char* __fastcall cl_get_config_string(int config_string_index)
	{
		if (constexpr auto max_index = 3629u; static_cast<std::uint32_t>(config_string_index) <= max_index)
		{
			return cl_get_config_string_hook.call<const char*>(config_string_index);
		}

		PRINT_LOG("(configStringIndex) > (MAX_CONFIGSTRINGS) [%u] [0x%llX]", config_string_index, game::derelocate(_ReturnAddress()));
		return "";
	}

	const char* __fastcall bg_cache_get_event_string_name_for_index(int inst, int index)
	{
		if (constexpr auto max_index = 0x100; static_cast<uint32_t>(index) <= max_index)
		{
			return bg_cache_get_event_string_name_for_index_hook.call<const char*>(inst, index);
		}

		PRINT_LOG("Crash attempt caught! [%u] [0x%llX]", index, game::derelocate(_ReturnAddress()));
		return "";
	}
	
	float __fastcall xanims_get_notetrack_time(const game::XAnim *anims, int animIndex, unsigned int name)
	{
		const auto valid = anims && anims->size > animIndex && !anims->entries[animIndex].numAnims && anims->entries[animIndex]._anon_0.parts;

		if (valid)
		{
			return xanims_get_notetrack_time_hook.call<float>(anims, animIndex, name);
		}

		PRINT_LOG("Crash attempt caught! [0x%llX]", game::derelocate(_ReturnAddress()));
		return -1.0f;
	}
	
	bool __fastcall dobj_get_bone_index(const void *obj, const int name, const uint16_t *index, int modelMask)
	{
		const auto valid = obj && name && index;

		if (valid)
		{
			return dobj_get_bone_index_hook.call<bool>(obj, name, index, modelMask);
		}

		return false;
	}

	uintptr_t __fastcall cg_get_player_fx_table(const uintptr_t cent)
	{
		if (!cent)
			return 0;
		
		const auto upper = *reinterpret_cast<uint32_t*>(cent + 0x4E8);
		const auto character_type = (upper >> 3) & 0x3F;

		if (!character_type)
			return 0;

		const auto mode = game::call<game::eModes>(0x7FF6C73D7790);
		const auto body_type = game::call<uintptr_t>(0x7FF6C538DD00, mode, character_type - 1);

		if (!body_type)
			return 0;

		auto body_index = (upper >> 9) & 0x3F;

		if (body_index)
		{
			body_index -= 1;

			if (body_index >= *reinterpret_cast<uint32_t*>(body_type + 0xA8))
				return 0;

			const auto body_info = *reinterpret_cast<uintptr_t*>(body_type + 0xB0);
			return *reinterpret_cast<uintptr_t*>(0xA8 * body_index + body_info + 0x90);
		}
		else
		{
			return *reinterpret_cast<uintptr_t*>(body_type + 0x108);
		}
	}

	utils::hook::detour CL_ParseServerMessage_hook;
	
	bool __fastcall CL_ParseServerMessage(LocalClientNum_t localClientNum,
		game::netadr_t from,
		game::msg_t *msg,
		int time,
		bool connectionlesss_packets_only)
	{
		if (menu::deez)
		{
			DEBUG_LOG("Called");
			return false;
		}
		
		return CL_ParseServerMessage_hook.call<bool>(localClientNum, from, msg, time, connectionlesss_packets_only);
	}
	
	void __fastcall sv_packet_event(game::netadr_t from, uint64_t xuid, game::msg_t* msg)
	{
		if ((game::LiveUser_IsXUIDLocalPlayer(xuid) || xuid == 0xDEADFA11) && !game::NET_CompareAdr(from, game::clc()->serverAddress))
		{
			PRINT_LOG("Received an invalid message from %s", utils::get_sender_string(from).data());
			return;
		}

		return sv_packet_event_hook.call(from, xuid, msg);
	}
	
	void __fastcall sv_update_server_commands_to_client(game::client_t* client, game::msg_t* msg)
	{
		if ((client->reliableSequence - client->reliableAcknowledge) < 0)
		{
			client->reliableAcknowledge = client->reliableSequence; 
			PRINT_LOG("Invalid reliableAcknowledge message from %s (%i %i)", utils::get_sender_string(client->address).data());
			return;
		}
		
		return sv_update_server_commands_to_client_hook.call<void>(client, msg);
	}
	
	void __fastcall cl_parse_gamestate(LocalClientNum_t localClientNum, game::msg_t* msg)
	{
		const auto server_id = game::cl()->serverId;
		const auto config_string = game::call<const char*>(0x7FF6C6601110, 3);
		const auto invalid = server_id && ((utils::atoi<uint8_t>(config_string) ^ static_cast<uint8_t>(server_id)) & 0xF0) == 0;

		if (invalid)
		{
			PRINT_LOG("Exploit attempt caught! [%i] [%s] [0x%llX]", server_id, config_string, game::derelocate(_ReturnAddress()));
			return;
		}
		
		return cl_parse_gamestate_hook.call<void>(localClientNum, msg);
	}
	
	bool netchan_save_fragment(game::NetChanMessage_s* msg)
	{
		if (game::s_fragmentFreeHead && !*game::s_fragmentFreeHead)
		{
			return true;
		}

		msg->lastTouchedMS = *reinterpret_cast<int*>(OFFSET(0x7FF6DC12BB1C));
		return false;
	}
	
	bool __fastcall netchan_send_message(ControllerIndex_t localControllerIndex, game::NetChanMsgType type, game::NetchanMsgMode mode, uint8_t* data, size_t length, uint64_t xuid, game::netadr_t* netadr, game::netchanMsgConfig_t* config)
	{
		if (game::s_fragmentFreeHead && !*game::s_fragmentFreeHead)
		{
			return false;
		}
		
		return netchan_send_message_hook.call<bool>(localControllerIndex, type, mode, data, length, xuid, netadr, config);
	}
	
	bool __fastcall handle_lobby_state_core(ControllerIndex_t controllerIndex, game::netadr_t adr, uint64_t xuid, uintptr_t data)
	{
		game::I_strncpyz(reinterpret_cast<char*>(data + 0x53A0), "", 32);
		game::I_strncpyz(reinterpret_cast<char*>(data + 0x5424), "", 32);
		*reinterpret_cast<int*>(data + 0x53C0) = 0;
		*reinterpret_cast<int*>(data + 0x5444) = 0;

		return handle_lobby_state_core_hook.call<bool>(controllerIndex, adr, xuid, data);
	}

	typedef void (*DDLWriteCB)(void*, void*);
	
	enum ClientContentType
	{
		CLIENT_CONTENT_TYPE_PAINTSHOP = 0x0,
		CLIENT_CONTENT_TYPE_LOADOUT = 0x1,
		MAX_CLIENT_CONTENT_TYPE = 0x2,
	};
	
	struct DDLContext
	{
		void* buff;
		int len;
		const void* def;
		DDLWriteCB writeCB;
		void* userData;
	};
	
	struct ClientContentState
	{
		DDLContext ddlCtx;
		uint32_t bufferChecksum;
		uint16_t compressedBufferSize;
		uint16_t cursorPosition;
		uint16_t totalContentReceived;
		bool isDataCompressed;
	};
	
	struct ClientContent
	{
		ClientContentState contentStates[2];
		int sessionMode;
		int gameMode;
		char buffer[163840];
	};
	
	void __fastcall ClientContent_SetData(ClientContentType contentType, ClientContent* clientContent, const void* data, uint32_t dataSize)
	{
		DEBUG_LOG("content type %i dataSize %i", contentType, dataSize);

		for (size_t i = 0; i < sizeof(ClientContentState); ++i)
		{
			DEBUG_LOG("bufferChecksum %i compressedBufferSize %i totalContentReceived %i", 
				clientContent->contentStates[i].bufferChecksum,
				clientContent->contentStates[i].compressedBufferSize, 
				clientContent->contentStates[i].totalContentReceived);
		}
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

		const auto cg_get_entity_impact_type_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			const auto ret = a.newLabel();
			
			a.test(rax, rax);
			a.jz(ret);
			
			a.mov(rcx, rax);
			a.xor_(eax, eax);
			a.jmp(OFFSET(0x7FF6C64B2560));

			a.bind(ret);
			a.jmp(OFFSET(0x7FF6C64B250D));
		});

		const auto netchan_save_fragment_stub = utils::hook::assemble([=](utils::hook::assembler& a)
		{
			const auto return_unhandled = a.newLabel(); 
				
			a.pushad64();
			a.mov(rcx, rbx);
			a.call_aligned(security::netchan_save_fragment);
			a.test(al, al);
			a.jz(return_unhandled);

			a.popad64();
			a.mov(r13, rbx);
			a.jmp(OFFSET(0x7FF6C7455F96));
			
			a.bind(return_unhandled);
			a.popad64(); 
			a.jmp(OFFSET(0x7FF6C74561B7));
		});
		
		utils::hook::iat("kernel32.dll", "IsProcessorFeaturePresent", is_processor_feature_present);
		utils::hook::iat("kernel32.dll", "UnhandledExceptionFilter", unhandled_exception_filter); 

		rb_draw_text_2d_cmd_hook.create(OFFSET(0x7FF6C6F3F5A0), rb_draw_text_2d_cmd);
		r_read_char_from_string_hook.create(OFFSET(0x7FF6C6F90F30), r_read_char_from_string);
		ui_cod_read_char_from_string_hook.create(OFFSET(0x7FF6C720E6C0), ui_cod_read_char_from_string);
		lua_cmd_parse_args_hook.create(OFFSET(0x7FF6C71E54E0), lua_cmd_parse_args);
		cl_rank_get_paragon_icon_name_hook.create(OFFSET(0x7FF6C66AAD20), cl_rank_get_paragon_icon_name);
		net_enqueue_packet_hook.create(OFFSET(0x7FF6C7458600), net_enqueue_packet);
		cl_get_config_string_hook.create(OFFSET(0x7FF6C6601110), cl_get_config_string);
		bg_cache_get_event_string_name_for_index_hook.create(OFFSET(0x7FF6C53878A0), bg_cache_get_event_string_name_for_index);
		xanims_get_notetrack_time_hook.create(OFFSET(0x7FF6C7622CC0), xanims_get_notetrack_time);
		dobj_get_bone_index_hook.create(OFFSET(0x7FF6C761DF70), dobj_get_bone_index);
		sv_packet_event_hook.create(OFFSET(0x7FF6C75351B0), sv_packet_event);
		sv_update_server_commands_to_client_hook.create(OFFSET(0x7FF6C753C690), sv_update_server_commands_to_client);
		cl_parse_gamestate_hook.create(OFFSET(0x7FF6C6644F60), cl_parse_gamestate);
		netchan_send_message_hook.create(OFFSET(0x7FF6C7456A60), netchan_send_message);
		handle_lobby_state_core_hook.create(OFFSET(0x7FF6C71B3570), handle_lobby_state_core);
		
		//lobby_msg_rw_package_u_char_hook.create(OFFSET(0x7FF6C71D6850), lobby_msg_rw_package_u_char);
		//CL_ParseServerMessage_hook.create(OFFSET(0x7FF6C6639E40), CL_ParseServerMessage);

		utils::hook::jump(OFFSET(0x7FF6C74561AE), netchan_save_fragment_stub);
		utils::hook::nop(OFFSET(0x7FF6C74561AE) + 5, 4);
		
		utils::hook::jump(OFFSET(0x7FF6C5563A40), cg_get_player_fx_table);
		utils::hook::jump(OFFSET(0x7FF6C64B255B), cg_get_entity_impact_type_stub);
		utils::hook::jump(OFFSET(0x7FF6C72F9F40), ui_model_get_model_from_path_stub);
		utils::hook::jump(OFFSET(0x7FF6C7213E78), OFFSET(0x7FF6C7213F6B)); // UI_DoModelStringReplacement
		utils::hook::jump(OFFSET(0x7FF6C73CF7B6), OFFSET(0x7FF6C73CF788)); // Cmd_TokenizeStringInternal

		utils::hook::set<uint8_t>(OFFSET(0x7FF6C71718EA) + 2, 18); // LivePresence_Serialize
		utils::hook::set<uint8_t>(OFFSET(0x7FF6C7171995) + 2, 18); // LivePresence_Serialize

		utils::hook::set<uint8_t>(OFFSET(0x7FF6C719D245), 0xEB); // LiveStorage_WriteUserFilesFailure
		
		utils::hook::retn(OFFSET(0x7FF6C72B3220)); // Lua_Engine_Function_OpenURL
		utils::hook::retn(OFFSET(0x7FF6C70100B0)); // hks::base_loadfile
		utils::hook::retn(OFFSET(0x7FF6C7011EE0)); // hks::base_load
		utils::hook::retn(OFFSET(0x7FF6C700CF00)); // hks::string_dump
		utils::hook::retn(OFFSET(0x7FF6C7014190)); // hks::debug
		utils::hook::retn(OFFSET(0x7FF6C7013510)); // hks::io
		utils::hook::retn(OFFSET(0x7FF6C7013D20)); // hks::os
		utils::hook::retn(OFFSET(0x7FF6C7014B10)); // hks::havokscript
		utils::hook::retn(OFFSET(0x7FF6C7014B40)); // hks::serialize
		utils::hook::retn(OFFSET(0x7FF6C70099C0)); // hks::package_loadlib
	}
}