#pragma once
#include "utils/string.hpp"
#include "utils/spoof_call/spoof_call.hpp"
#include "structs.hpp"
#include "offsets.hpp"

namespace game
{
	enum class bone_tag
	{
		helmet,
		head_end,
		head,
		neck,
		shoulder_left,
		shoulder_right,
		clavicle_left,
		clavicle_right,
		spineupper,
		spine,
		spinelower,
		hip_left,
		hip_right,
		knee_left,
		knee_right,
		wrist_left,
		wrist_right,
		wrist_twist_left,
		wrist_twist_right,
		elbow_left,
		elbow_right,
		elbow_buldge_left,
		elbow_buldge_right,
		ankle_left,
		ankle_right,
		ball_left,
		ball_right,
		mainroot,
		num_tags,
	}; 
	
	namespace net
	{
		namespace netchan
		{
			struct write_packet
			{
				uint8_t server_id;
				int message_sequence;
				int command_sequence;
				int acknowledge;
			}; 
			
			bool send(const NetChanMsgType type, const std::string& data, const netadr_t& netadr, const uint64_t target_xuid, const uint64_t sender_xuid = 0);
			bool send_oob(const uint64_t xuid, const netadr_t& netadr, const std::string& data, const bool fill = false);
			bool write(const write_packet& packet, const std::string& data, const netadr_t& netadr, const uint64_t target_xuid, const uint64_t sender_xuid = 0);
			bool write(const std::string& data);
		}
		
		namespace oob
		{
			bool send(const netadr_t& target, const std::string& data);
			bool register_remote_addr(const XSESSION_INFO* info, netadr_t* addr);
			bool register_remote_addr(const InfoResponseLobby& lobby, netadr_t* addr);
		}
		
		bool send(const netadr_t& netadr, const std::string& data);
	}
	
	size_t get_base(); 
	void initialize();
	bool in_game();
	int find_target_from_addr(const LobbySession* session, const netadr_t& from);
	bool can_connect_to_player(const LobbySession* session, const size_t client_num, const size_t target_xuid);
	XSESSION_INFO get_session_info(const InfoResponseLobby& lobby);
	void on_every_frame();
	bool is_enemy(const int client_num);
	bool CG_WorldPosToScreenPos(const Vec3* pos, Vec2* out);
	Vec2 get_screen_pos(const Vec3& world_pos);
	Vec2 get_scale(const centity_t * cent, const float pos);
	Vec3 get_top_position(const centity_t * cent);
	bool AimTarget_GetTagPos(const centity_t * cent, const scr_string_t & tag_name, Vec3 * end);
	bool CG_GetPlayerViewOrigin(const playerState_s* ps, Vec3* view_origin);
	void adjust_user_cmd_movement(usercmd_s * cmd_old, const float angle, const float old_angle);
	void adjust_user_cmd_movement(const usercmd_s * cmd, usercmd_s * cmd_old, const float yaw);
	float get_weapon_damage_range(const Weapon & weapon);
	bool CG_BulletTrace(BulletTraceResults * br, BulletFireParams * bp, const int attacker_entity_num, int lastSurfaceType);
	bool send_instant_message(const std::vector<std::uint64_t>& recipients, const std::uint8_t type, const void * message, const std::uint32_t message_size);
	bool send_instant_message(const std::vector<std::uint64_t>& recipients, const std::uint8_t type, const std::string & data);
	bool send_instant_message(const std::vector<std::uint64_t>& recipients, const std::uint8_t type, const msg_t & msg);
	const char* LobbyTypes_GetMsgTypeName(const MsgType type);
	int LobbySession_GetClientNumByXuid(const game::LobbySession* session, const std::uint64_t xuid);
	bool is_valid_lobby_type(const int lobby_type);
	void enum_assets(const XAssetType type, const std::function<void(XAssetHeader)>& callback, const bool includeOverride);
	bool LobbyMsgRW_PackageElement(msg_t * msg, bool add_element);
	netadr_t get_session_netadr(const LobbySession* session, const ActiveClient* client);
	char * I_strncpyz(char * place, const std::string & string, const size_t length);
	TaskRecord* TaskManager2_SetupRemoteTask(const TaskDefinition* definition, bdRemoteTask* remote_task, const uint32_t timeout = 0);
	
	extern std::array<scr_string_t, static_cast<std::uint32_t>(bone_tag::num_tags)> bone_tags;

	const static auto base_address = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr)) + 0x1000;

	const static auto LobbyJoinSource_IMInfoResponse = reinterpret_cast<bool(*)(const ControllerIndex_t, const uint64_t, msg_t*)>(base_address + 0x1EE4E00);
	const static auto LobbyJoinSource_IMInfoRequest = reinterpret_cast<bool(*)(const ControllerIndex_t, const uint64_t, uint32_t)>(base_address + 0x1EE1620);
	const static auto LobbyMsgRW_IsEndOfArray = reinterpret_cast<bool(*)(msg_t*)>(base_address + 0x1EF5520);
	const static auto CG_SimulateBulletFire_EndPos = reinterpret_cast<void(*)(int*, float, float, const Vec3*, Vec3*, Vec3*, float, float, const Vec3*, const Vec3*, const Vec3*, float, Weapon, int, int)>(base_address + 0x123CB50);
	const static auto Cmd_AddCommandInternal = reinterpret_cast<void(*)(const char *, void(__cdecl *function)(), cmd_function_s *)>(base_address + 0x20EC530);
	const static auto DB_GetXAssetName = reinterpret_cast<const char*(*)(const XAsset*)>(base_address + 0x13E8DA0);
	const static auto BG_GetDepth = reinterpret_cast<float(*)(BulletHitInfo*, const BulletTraceResults*, const BulletFireParams*, const BulletTraceResults*)>(base_address + 0xA1CF0);
	const static auto BG_GetMinDamageRangeScaled = reinterpret_cast<float(*)(Weapon)>(base_address + 0x26F0960);
	const static auto BG_GetMultishotBaseMinDamageRangeScaled = reinterpret_cast<float(*)(Weapon)>(base_address + 0x26F0550);
	const static auto BG_GetSpreadForWeapon = reinterpret_cast<void(*)(playerState_s const*, const Weapon, float*, float*)>(base_address + 0x26D0310);
	const static auto BG_seedRandWithGameTime = reinterpret_cast<int(*)(int*)>(base_address + 0x2687670);
	const static auto AngleVectors = reinterpret_cast<void(*)(const Vec3 *, Vec3 *, Vec3 *, Vec3 *)>(base_address + 0x22AA9E0); 
	const static auto ClampChar = reinterpret_cast<char(*)(const int i)>(base_address + 0x22A52B0);
	const static auto CG_ClientHasPerk = reinterpret_cast<bool(*)(LocalClientNum_t, ClientNum_t, unsigned int)>(base_address + 0x9425D0);
	const static auto BG_GetWeaponDef = reinterpret_cast<WeaponDef*(*)(Weapon)>(base_address + 0x26E9B80);
	const static auto BG_GetPenetrateType = reinterpret_cast<PenetrateType(*)(const Weapon)>(base_address + 0x26F0ED0);
	const static auto BG_GetSurfacePenetrationDepth = reinterpret_cast<float(*)(PenetrateType penetrateType, int surfaceType)>(base_address + 0x26D0480);
	const static auto BG_AdvanceTrace = reinterpret_cast<bool(*)(BulletFireParams* bp, BulletTraceResults*, float)>(base_address + 0xA1020); 
	const static auto BG_IsDualWield = reinterpret_cast<bool(*)(const Weapon)>(base_address + 0x26F3820);
	const static auto BG_GetViewmodelWeaponIndex = reinterpret_cast<Weapon(*)(const playerState_s*)>(base_address + 0x26D04A0);
	const static auto BG_GetWeaponDamageForRange = reinterpret_cast<int(*)(const Weapon, const Vec3*, const Vec3*)>(base_address + 0x26F27A0);
	const static auto BG_GetWeaponHitLocationMultiplier = reinterpret_cast<float(*)(const Weapon, uint16_t)>(base_address + 0x26F2B80);
	const static auto G_GetWeaponHitLocationMultiplier = reinterpret_cast<float(*)(uint16_t, Weapon)>(base_address + 0x19863C0);
	const static auto Trace_GetEntityHitId = reinterpret_cast<std::uint16_t(__fastcall*)(const trace_t*)>(base_address + 0x20E4AD0);
	const static auto vectoangles = reinterpret_cast<void(*)(const Vec3*, Vec3*)>(base_address + 0x22AA710);
	const static auto Com_GetClientDObj = reinterpret_cast<void*(__fastcall*)(int, LocalClientNum_t)>(base_address + 0x214DBA0);
	const static auto GScr_AllocString = reinterpret_cast<scr_string_t(__fastcall*)(const char*)>(base_address + 0x1A82520);
	const static auto CG_DObjGetWorldTagPos = reinterpret_cast<bool(__fastcall*)(const cpose_t *, void*, scr_string_t, Vec3*)>(base_address + 0x1AAD50);
	const static auto LobbyClientLaunch_IsInGame = *reinterpret_cast<bool(__fastcall*)()>(base_address + 0x1ECD6D0);
	const static auto dwRegisterSecIDAndKey = reinterpret_cast<bool(*)(const bdSecurityID *, const bdSecurityKey *)>(base_address + 0x143D120);
	const static auto dwCommonAddrToNetadr = reinterpret_cast<bool(*)(netadr_t *const, const void *const, const bdSecurityID *)>(base_address + 0x143B360);
	const static auto MSG_InfoResponse = reinterpret_cast<bool(__fastcall *)(void*, msg_t *)>(base_address + 0x1EE0EB0);
	const static auto LobbyMsgRW_PackageShort = reinterpret_cast<bool(*)(game::msg_t *, const char*, short*)>(game::base_address + 0x1EF57A0);
	const static auto LobbyMsgRW_PackageBool = reinterpret_cast<bool(*)(game::msg_t *, const char*, bool*)>(game::base_address + 0x1EF55D0);
	const static auto LobbyMsgRW_PackageString = reinterpret_cast<bool(*)(game::msg_t *, const char*, char*, int)>(game::base_address + 0x1EF57C0);
	const static auto LobbyMsgRW_PackageUShort = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*, std::uint16_t*)>(base_address + 0x1EF5970);
	const static auto LobbyMsgRW_PackageUChar = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*, std::uint8_t*)>(base_address + 0x1EF5850);
	const static auto LobbyMsgRW_PackageUInt64 = reinterpret_cast<bool(__fastcall *)(msg_t *, const char *, std::uint64_t*)>(base_address + 0x1EF5870);
	const static auto LobbyMsgRW_PackageUInt = reinterpret_cast<bool(__fastcall *)(msg_t *, const char *, std::uint32_t*)>(base_address + 0x1EF58F0);
	const static auto LobbyMsgRW_PackageXuid = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*, std::uint64_t*)>(base_address + 0x1EF5990);
	const static auto LobbyMsgRW_PackageArrayStart = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*)>(base_address + 0x1EF5560);
	const static auto LobbyMsgRW_PackageInt = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*, int*)>(base_address + 0x1EF5720);
	const static auto LobbyMsgRW_PackageGlob = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*, void*, int)>(base_address + 0x1EF5700);
	const static auto LobbyMsgRW_PrepReadMsg = reinterpret_cast<bool(__fastcall *)(msg_t *, char *, int)>(base_address + 0x1EF5A10);
	const static auto LiveUser_IsXUIDLocalPlayer = reinterpret_cast<bool(__fastcall*)(const std::uint64_t)>(base_address + 0x1EC61C0);
	const static auto dwNetadrToCommonAddr = reinterpret_cast<bool(__fastcall*)(netadr_t, void*, const unsigned int, bdSecurityID*)>(base_address + 0x143CA60);
	const static auto Sys_GetTLS = reinterpret_cast<TLSData*(__fastcall*)()>(base_address + 0x2183210);
	const static auto MSG_ReadData = reinterpret_cast<void(*)(msg_t*, void*, int)>(base_address + 0x2154F10); 
	const static auto MSG_Init = reinterpret_cast<void(*)(msg_t*, char*, int)>(base_address + 0x21549E0);
	const static auto MSG_InitReadOnly = reinterpret_cast<void(*)(msg_t*, std::uint8_t*, int)>(base_address + 0x2154A70);
	const static auto MSG_BeginReading = reinterpret_cast<void(*)(msg_t*)>(base_address + 0x2154760);
	const static auto LiveUser_GetXuid = reinterpret_cast<std::uint64_t(*)(const ControllerIndex_t)>(base_address + 0x1EC63A0);
	const static auto CL_AddReliableCommand = reinterpret_cast<bool(*)(LocalClientNum_t, const char*)>(base_address + 0x135C4C0);
	const static auto Cmd_ExecuteSingleCommand = reinterpret_cast<void(__fastcall*)(LocalClientNum_t, ControllerIndex_t, const char *, bool)>(base_address + 0x20ECC20);
	const static auto Cbuf_AddText = reinterpret_cast<void(__fastcall*)(LocalClientNum_t, const char*)>(base_address + 0x20EB8B0);
	const static auto dwInstantSendMessage = reinterpret_cast<bool(*)(ControllerIndex_t, const std::uint64_t*, unsigned int, char, const void *, unsigned int)>(base_address + 0x1439810);
	const static auto Live_IsUserSignedInToDemonware = reinterpret_cast<bool(*)(const ControllerIndex_t)>(base_address + 0x1E0C830);
	const static auto CL_GetClientName = reinterpret_cast<bool(*)(LocalClientNum_t, int, char*, int, bool)>(base_address + 0x13E2140);
	const static auto NET_CompareAdr = reinterpret_cast<bool(*)(const netadr_t, const netadr_t)>(base_address + 0x21722D0);
	const static auto CL_GetLocalClientGlobals = *reinterpret_cast<clientActive_t*(*)(LocalClientNum_t)>(base_address + 0x70BD0);
	const static auto LivePresence_Serialize = reinterpret_cast<int(*)(bool, PresenceData *, const void *, size_t)>(base_address + 0x1E92D80);
	const static auto task_livepresence_dw_get = reinterpret_cast<game::TaskDefinition*>(base_address + 0x3018A40);
	const static auto TaskManager2_TaskGetInProgress = reinterpret_cast<bool(*)(const void*)>(base_address + 0x22B0780);
	const static auto TaskManager2_SetupNestedTask = reinterpret_cast<TaskRecord*(*)(const void*, ControllerIndex_t, TaskRecord*, void*)>(base_address + 0x22B0380);
	const static auto dwPresenceGet = reinterpret_cast<TaskRecord*(*)(const ControllerIndex_t, dwPresenceTask *const)>(base_address + 0x143D7B0);;
	const static auto TaskManager2_CreateTask = reinterpret_cast<TaskRecord*(*)(const void*, const ControllerIndex_t, TaskRecord*, int)>(base_address + 0x22AF770);
	const static auto TaskManager2_StartTask = reinterpret_cast<void(*)(TaskRecord*)>(base_address + 0x22B06C0);
	const static auto& g_fHitLocDamageMult = reinterpret_cast<float*>(base_address + 0xA08C4D0);
	const static auto& g_assetNames = reinterpret_cast<const char**>(base_address + 0x3329080);
	const static auto& s_infoProbe = reinterpret_cast<InfoProbe*>(base_address + 0x1574B590);
	const static auto& window = *reinterpret_cast<HWND*>(base_address + 0x17E773D0);
	const static auto& lobbyMsgName = reinterpret_cast<const char**>(base_address + 0x34104A0);
	const static auto& s_presenceTaskData = reinterpret_cast<dwPresenceTask*>(base_address + 0x1140F0A8 + 0x14 * 0);
	
	inline LobbySession* get_host_session(const int lobby_type)
	{
		if (!is_valid_lobby_type(lobby_type))
		{
			return nullptr;
		}

		return reinterpret_cast<LobbySession*>(base_address + 0x1567D380 + 0x66828ull * lobby_type);
	}
	
	inline LobbySession* get_client_session(const int lobby_type)
	{
		if (!is_valid_lobby_type(lobby_type))
		{
			return nullptr;
		}
		
		return reinterpret_cast<LobbySession*>(base_address + 0x15676490 + sizeof LobbySession * lobby_type);
	}

	inline LobbySession* session_data()
	{
		const auto party_session = game::get_client_session(game::LOBBY_TYPE_PRIVATE); 
		const auto lobby_session = game::get_client_session(game::LOBBY_TYPE_GAME);

		return lobby_session->active ? lobby_session : party_session;
	}

	inline clientActive_t* cl()
	{
		return spoof_call::call(CL_GetLocalClientGlobals, 0u);
	}

	inline clientConnection_t* clc()
	{
		return reinterpret_cast<clientConnection_t*>(*reinterpret_cast<std::uintptr_t*>(game::base_address + 0x53D9BB8));
	}
	
	inline cg_t* cg() 
	{
		const auto& decrypted_cl_ptr = cl(); 
		
		if (decrypted_cl_ptr == nullptr)
			return nullptr;
		
		const auto decrypted_cg_ptr = reinterpret_cast<std::uintptr_t>(decrypted_cl_ptr) - 0x15CE0B0;
		return reinterpret_cast<cg_t*>(decrypted_cg_ptr);
	}

	inline centity_t* centity(const int index) 
	{
		const auto& decrypted_cg_ptr = cg();

		if (decrypted_cg_ptr == nullptr)
			return nullptr;
		
		const auto decrypted_centity_ptr = reinterpret_cast<std::uintptr_t>(decrypted_cg_ptr) + 0x8AAAE0;
		return reinterpret_cast<centity_t*>(decrypted_centity_ptr + sizeof centity_t * index);
	}

	inline auto get_tag(const bone_tag t)
	{
		if (t >= bone_tag::num_tags)
			return static_cast<scr_string_t>(0u);

		return bone_tags[static_cast<std::size_t>(t)];
	}

	template <typename T = void, typename... Args>
	inline T call(const uintptr_t address, Args ... args)
	{
		return reinterpret_cast<T(*)(Args ...)>(address)(args...);
	}

	template <typename T = void, typename... Args>
	inline T call(const void* address, const size_t index, Args ... args)
	{
		const auto table = *reinterpret_cast<uintptr_t**>(uintptr_t(address));
		return game::call<T>(table[index], address, args...);
	}

	inline size_t relocate(const size_t val)
	{
		const auto base = get_base();
		return base + (val - 0x140000000);
	}

	inline size_t derelocate(const size_t val)
	{
		const auto base = get_base();
		return (val - base) + 0x140000000;
	}

	inline size_t derelocate(const void* val)
	{
		return derelocate(reinterpret_cast<size_t>(val));
	}
}

inline size_t operator"" _g(const size_t val)
{
	return game::relocate(val);
}
