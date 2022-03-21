#pragma once
#include "structs.hpp"
#include "offsets.hpp"
#include "core/logger/logger.hpp"
#include "utils/utils.hpp"
#include "utils/spoof_call/spoof_call.hpp"

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
	
	namespace oob
	{
		bool send(const netadr_t & target, const std::string & data);
		bool register_remote_addr(const XSESSION_INFO * info, netadr_t * addr);
		bool register_remote_addr(const InfoResponseLobby & lobby, netadr_t * addr);
		bool send_lobby_msg(const netadr_t& to, const msg_t& msg, const LobbyModule module = LOBBY_MODULE_HOST);
	}
	
	void initialize();
	bool in_game();
	int find_target_from_addr(const netadr_t & from);
	bool can_connect_to_player(const size_t client_num, const size_t target_xuid);
	XSESSION_INFO get_session_info(const InfoResponseLobby& lobby);
	void on_every_frame();
	bool is_enemy(const int client_num);
	bool CG_WorldPosToScreenPos(const Vec3* pos, Vec2* out);
	Vec2 get_screen_pos(const Vec3 & world_pos);
	Vec2 get_scale(const centity_t * cent, const float pos);
	Vec3 get_top_position(const centity_t * cent);
	bool AimTarget_GetTagPos(const centity_t * cent, const scr_string_t & tag_name, Vec3 * end);
	bool CG_GetPlayerViewOrigin(const playerState_s* ps, Vec3* view_origin);
	void adjust_user_cmd_movement(usercmd_s * cmd_old, const float angle, const float old_angle);
	void adjust_user_cmd_movement(const usercmd_s * cmd, usercmd_s * cmd_old, const float yaw);
	float get_weapon_damage_range(const Weapon & weapon);
	bool CG_BulletTrace(BulletTraceResults * br, BulletFireParams * bp, const int attacker_entity_num, int lastSurfaceType);
	bool is_valid_target(const int client_num);
	bool send_instant_message(const std::vector<std::uint64_t>& recipients, const std::uint8_t type, const void * message, const std::uint32_t message_size);
	bool send_instant_message(const std::vector<std::uint64_t>& recipients, const std::uint8_t type, const msg_t & msg);
	bool send_netchan_message(const netadr_t& netadr, const std::uint64_t xuid, const std::string& data);
	SessionClient* LobbySession_GetClientByXuid(const std::uint64_t xuid);
	int LobbySession_GetClientNumByXuid(const std::uint64_t xuid);
	bool is_valid_lobby_type(const int lobby_type);
	void enum_assets(const XAssetType type, const std::function<void(XAssetHeader)>& callback, const bool includeOverride);
	bool LobbyMsgRW_PackageElement(msg_t * msg, bool add_element);
	
	extern std::unordered_map<std::string, bool> handlers;
	extern std::array<scr_string_t, static_cast<std::uint32_t>(bone_tag::num_tags)> bone_tags;
	extern LobbySession* session;

	const static auto base_address = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr)) + 0x1000;

	const static auto I_CleanStr = reinterpret_cast<char*(*)(const char*)>(base_address + 0x22E8C10);
	const static auto I_CleanChat = reinterpret_cast<char*(*)(const char*)>(base_address + 0x22E8B80);
	const static auto LobbyJoinSource_IMInfoResponse = reinterpret_cast<bool(*)(const ControllerIndex_t, const uint64_t, msg_t*)>(base_address + 0x1EE4E00);
	const static auto LobbyJoinSource_IMInfoRequest = reinterpret_cast<bool(*)(const ControllerIndex_t, const uint64_t, uint32_t)>(base_address + 0x1EE1620);
	const static auto Sys_GetCurrentThreadName = reinterpret_cast<const char*(*)()>(base_address + 0x21831A0);
	const static auto LobbyMsgRW_IsEndOfArray = reinterpret_cast<bool(*)(msg_t*)>(base_address + 0x1EF5520);
	const static auto AngleDelta = reinterpret_cast<float(*)(const float, const float)>(base_address + 0x22A4800);
	const static auto AngleNormalize360 = reinterpret_cast<float(*)(const float)>(base_address + 0x22A4850);
	const static auto AngleNormalize180 = reinterpret_cast<float(*)(const float)>(base_address + 0x701A0);
	const static auto CG_SimulateBulletFire_EndPos = reinterpret_cast<void(*)(int*, float, float, const Vec3*, Vec3*, Vec3*, float, float, const Vec3*, const Vec3*, const Vec3*, float, Weapon, int, int)>(base_address + 0x123CB50);
	const static auto DB_EnumXAssets = reinterpret_cast<void(*)(XAssetType type, void(*)(XAssetHeader, void*), const void*, bool)>(base_address + 0x141F970);
	const static auto Cmd_AddCommandInternal = reinterpret_cast<void(*)(const char *, void(__cdecl *function)(), cmd_function_s *)>(base_address + 0x20EC530);
	const static auto DB_GetXAssetName = reinterpret_cast<const char*(*)(const XAsset*)>(base_address + 0x13E8DA0);
	const static auto Material_RegisterHandle = reinterpret_cast<Material*(*)(const char *, int, bool, int)>(base_address + 0x1CD3B90);
	const static auto Com_SessionMode_IsMode = reinterpret_cast<bool(*)(eModes)>(base_address + 0x20F6DD0);
	const static auto MSG_LobbyStateGame = reinterpret_cast<bool(__fastcall *)(void*, msg_t*)>(base_address + 0x1ED4810);
	const static auto MSG_JoinAgreementRequest = reinterpret_cast<bool(__fastcall *)(void*, msg_t*)>(base_address + 0x1EE0F90);
	const static auto MSG_ClientReliableData = reinterpret_cast<bool(__fastcall *)(void*, msg_t*)>(base_address + 0x1ED3EF0);
	const static auto MSG_ClientContent = reinterpret_cast<bool(__fastcall *)(Msg_ClientContent*, msg_t*)>(base_address + 0x1ED3DF0);
	const static auto MSG_JoinParty = reinterpret_cast<bool(__fastcall *)(void*, msg_t*)>(base_address + 0x1EE1110);
	const static auto BG_GetDepth = reinterpret_cast<float(*)(BulletHitInfo*, const BulletTraceResults*, const BulletFireParams*, const BulletTraceResults*)>(base_address + 0xA1CF0);
	const static auto BG_GetMinDamageRangeScaled = reinterpret_cast<float(*)(Weapon)>(base_address + 0x26F0960);
	const static auto BG_GetMultishotBaseMinDamageRangeScaled = reinterpret_cast<float(*)(Weapon)>(base_address + 0x26F0550);
	const static auto sv_bullet_range = *reinterpret_cast<dvar_t**>(base_address + 0x176F96C0);
	const static auto BG_GetSpreadForWeapon = reinterpret_cast<void(*)(playerState_s const*, const Weapon, float*, float*)>(base_address + 0x26D0310);
	const static auto BG_seedRandWithGameTime = reinterpret_cast<int(*)(int*)>(base_address + 0x2687670);
	const static auto AngleVectors = reinterpret_cast<void(*)(const Vec3 *, Vec3 *, Vec3 *, Vec3 *)>(base_address + 0x22AA9E0); 
	const static auto ClampChar = reinterpret_cast<char(*)(const int i)>(base_address + 0x22A52B0);
	const static auto sv_penetrationCount = *reinterpret_cast<dvar_t**>(base_address + 0x176F96A8);
	const static auto bg_bulletPenetrationTreatVoidsAsSolid = *reinterpret_cast<dvar_t**>(base_address + 0x19C2BA30);
	const static auto CG_ClientHasPerk = reinterpret_cast<bool(*)(LocalClientNum_t, ClientNum_t, unsigned int)>(base_address + 0x9425D0);
	const static auto BG_GetWeaponDef = reinterpret_cast<WeaponDef*(*)(Weapon)>(base_address + 0x26E9B80);
	const static auto BG_GetPenetrateType = reinterpret_cast<PenetrateType(*)(const Weapon)>(base_address + 0x26F0ED0);
	const static auto BG_FixupHitInfo = reinterpret_cast<bool(*)(BulletPenetrationInfo*, int, int, int, int, float *, float, PenetrateType)>(base_address + 0xA1B30);
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
	const static auto LobbyLaunch_IsInGame = *reinterpret_cast<bool(__fastcall*)(LobbySession*)>(base_address + 0x1EE9EA0);
	const static auto LobbyHostLaunch_IsInGame = *reinterpret_cast<bool(__fastcall*)()>(base_address + 0x1EDCD20);
	const static auto LobbyClientLaunch_IsInGame = *reinterpret_cast<bool(__fastcall*)()>(base_address + 0x1ECD6D0);
	const static auto dwRegisterSecIDAndKey = reinterpret_cast<bool(*)(const bdSecurityID *, const bdSecurityKey *)>(base_address + 0x143D120);
	const static auto dwCommonAddrToNetadr = reinterpret_cast<bool(*)(netadr_t *const, const void *const, const bdSecurityID *)>(base_address + 0x143B360);
	const static auto MSG_InfoResponse = reinterpret_cast<bool(__fastcall *)(void*, msg_t *)>(base_address + 0x1EE0EB0);
	const static auto LobbyMsgRW_PackageUShort = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*, std::uint16_t*)>(base_address + 0x1EF5970);
	const static auto LobbyMsgRW_PackageUChar = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*, std::uint8_t*)>(base_address + 0x1EF5850);
	const static auto LobbyMsgRW_PrepWriteMsg = reinterpret_cast<bool(__fastcall *)(msg_t *, char *, int, MsgType)>(base_address + 0x1EF5A80);
	const static auto LobbyMsgRW_PackageUInt64 = reinterpret_cast<bool(__fastcall *)(msg_t *, const char *, std::uint64_t*)>(base_address + 0x1EF5870);
	const static auto LobbyMsgRW_PackageUInt = reinterpret_cast<bool(__fastcall *)(msg_t *, const char *, std::uint32_t*)>(base_address + 0x1EF58F0);
	const static auto LobbyMsgRW_PackageXuid = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*, std::uint64_t*)>(base_address + 0x1EF5990);
	const static auto LobbyMsgRW_PackageArrayStart = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*)>(base_address + 0x1EF5560);
	const static auto LobbyMsgRW_PackageInt = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*, int*)>(base_address + 0x1EF5720);
	const static auto LobbyMsgRW_PackageGlob = reinterpret_cast<bool(__fastcall *)(msg_t *, const char*, void*, int)>(base_address + 0x1EF5700);
	const static auto LobbyMsgRW_PrepReadMsg = reinterpret_cast<bool(__fastcall *)(msg_t *, msg_t*)>(base_address + 0x1EF5A40);
	const static auto LobbyMsgRW_PrepReadData = reinterpret_cast<bool(__fastcall *)(msg_t *, char *, int)>(base_address + 0x1EF5A10);
	const static auto LiveUser_IsXUIDLocalPlayer = reinterpret_cast<bool(__fastcall*)(const std::uint64_t)>(base_address + 0x1EC61C0);
	const static auto dwNetadrToCommonAddr = reinterpret_cast<bool(__fastcall*)(netadr_t, void*, const unsigned int, bdSecurityID*)>(base_address + 0x143CA60);
	const static auto Com_Error = reinterpret_cast<void(__fastcall*)(const char*, int, errorCode, const char*)>(base_address + 0x20F7BD0);
	const static auto Sys_GetTLS = reinterpret_cast<TLSData*(__fastcall*)()>(base_address + 0x2183210);
	const static auto LobbyTypes_GetLobbyTypeName = reinterpret_cast<const char*(__fastcall*)(const LobbyType)>(base_address + 0x1EEACE0);
	const static auto LobbyTypes_GetMsgTypeName = reinterpret_cast<const char*(__fastcall*)(const MsgType)>(base_address + 0x1EEAD60);
	const static auto MSG_ReadData = reinterpret_cast<void(*)(msg_t*, void*, int)>(base_address + 0x2154F10); 
	const static auto MSG_Init = reinterpret_cast<void(*)(msg_t*, char*, int)>(base_address + 0x21549E0);
	const static auto MSG_InitReadOnly = reinterpret_cast<void(*)(msg_t*, std::uint8_t*, int)>(base_address + 0x2154A70);
	const static auto MSG_BeginReading = reinterpret_cast<void(*)(msg_t*)>(base_address + 0x2154760);
	const static auto MSG_ReadByte = reinterpret_cast<std::uint8_t(*)(msg_t *)>(base_address + 0x2154EB0);
	const static auto MSG_WriteString = reinterpret_cast<void(*)(msg_t*, const char*)>(base_address + 0x2157C80);
	const static auto MSG_WriteShort = reinterpret_cast<void(*)(msg_t*, int)>(base_address + 0x2172130);
	const static auto MSG_WriteByte = reinterpret_cast<void(*)(msg_t*, int)>(base_address + 0x2157220);
	const static auto MSG_WriteData = reinterpret_cast<void(*)(msg_t*, const void *, int)>(base_address + 0x2157240);
	const static auto CG_BoldGameMessageCenter = reinterpret_cast<void(*)(LocalClientNum_t, const char *, int)>(base_address + 0x8C3C80);
	const static auto LiveUser_GetClientName = reinterpret_cast<const char*(*)(const ControllerIndex_t)>(base_address + 0x1EC6370);
	const static auto LiveUser_GetXuid = reinterpret_cast<std::uint64_t(*)(const ControllerIndex_t)>(base_address + 0x1EC63A0);
	const static auto CL_AddReliableCommand = reinterpret_cast<bool(*)(LocalClientNum_t, const char*)>(base_address + 0x135C4C0);
	const static auto Cmd_ExecuteSingleCommand = reinterpret_cast<void(__fastcall*)(LocalClientNum_t, ControllerIndex_t, const char *, bool)>(base_address + 0x20ECC20);
	const static auto Cbuf_AddText = reinterpret_cast<void(__fastcall*)(LocalClientNum_t, const char*)>(base_address + 0x20EB8B0);
	const static auto LobbyClientMsg_SendModifiedStats = reinterpret_cast<bool(*)(const ControllerIndex_t, const LobbyType)>(base_address + 0x1ED32A0);
	const static auto LiveSteam_PopOverlayForSteamID = reinterpret_cast<void(*)(std::uint64_t)>(base_address + 0x1EB0010);
	const static auto send_steam_message = reinterpret_cast<bool(*)(std::uint64_t, char, const void *, unsigned int)>(base_address + 0x1EAF250);
	const static auto dwInstantSendMessage = reinterpret_cast<bool(*)(ControllerIndex_t, const std::uint64_t*, unsigned int, char, const void *, unsigned int)>(base_address + 0x1439810);
	const static auto NET_OutOfBandPrint = reinterpret_cast<bool(*)(netsrc_t, netadr_t, const char *)>(base_address + 0x2173170);
	const static auto NET_OutOfBandData = reinterpret_cast<bool(*)(netsrc_t, netadr_t, const char *, int)>(base_address + 0x2173060);
	const static auto Live_IsDemonwareFetchingDone = reinterpret_cast<bool(*)(const ControllerIndex_t)>(base_address + 0x1E0C7A0);
	const static auto Com_IsInGame = reinterpret_cast<bool(*)()>(base_address + 0x2147D20);
	const static auto Com_IsRunningUILevel = reinterpret_cast<bool(*)()>(base_address + 0x2147DB0);
	const static auto CL_GetClientName = reinterpret_cast<bool(*)(LocalClientNum_t, int, char*, int, bool)>(base_address + 0x13E2140);
	const static auto NET_CompareAdr = reinterpret_cast<bool(*)(const netadr_t, const netadr_t)>(base_address + 0x21722D0);
	const static auto LiveSteam_GetFriends = reinterpret_cast<ISteamFriends*(*)(std::uintptr_t)>(base_address + 0x1DF99F0);
	const static auto LiveSteam_GetUser = reinterpret_cast<ISteamUser*(*)(std::uintptr_t)>(base_address + 0x1D27800);
	const static auto LiveSteam_GetApps = reinterpret_cast<ISteamApps*(*)(std::uintptr_t)>(base_address + 0x1EB04F0);
	const static auto CL_GetLocalClientGlobals = *reinterpret_cast<clientActive_t*(*)(LocalClientNum_t)>(base_address + 0x70BD0);
	const static auto CG_GetPredictedPlayerState = *reinterpret_cast<playerState_s*(*)(LocalClientNum_t)>(base_address + 0x8DE6A0);
	const static auto LobbySession_GetSession_Internal = *reinterpret_cast<LobbySession*(*)(LobbyType)>(base_address + 0x1ECCAB0);
	const static auto lobbymsg_prints = reinterpret_cast<dvar_t**>(base_address + 0x1574E840);
	const static auto LobbyMsgTransport_SendToAdr = reinterpret_cast<bool(*)(ControllerIndex_t, NetChanMsgType, LobbyModule, netadr_t, uint64_t, const msg_t *, MsgType)>(base_address + 0x1EF7850);
	const static auto LobbyNetChan_GetLobbyChannel = reinterpret_cast<NetChanMsgType(*)(LobbyType, LobbyChannel)>(base_address + 0x1EF7F70);
	const static auto Netchan_SendMessage = reinterpret_cast<bool(*)(ControllerIndex_t, NetChanMsgType, NetchanMsgMode, const char *, unsigned int, std::uint64_t, netadr_t, const char *)>(base_address + 0x2175A60);
	const static auto MSG_LobbyClientHeartbeat = *reinterpret_cast<bool(*)(Msg_LobbyClientHeartbeat*, msg_t*)>(base_address + 0x1ED4170);
	const static auto MSG_LobbyHostHeartbeat = *reinterpret_cast<bool(*)(Msg_LobbyHostHeartbeat*, msg_t*)>(base_address + 0x1ED4230);
	const static auto LobbyVM_JoinEvent = reinterpret_cast<void(*)(const ControllerIndex_t, uint64_t, JoinType)>(base_address + 0x1EEE8E0);
	const static auto& g_fHitLocDamageMult = reinterpret_cast<float*>(base_address + 0xA08C4D0);
	const static auto& g_assetNames = reinterpret_cast<const char**>(base_address + 0x3329080);
	//const static auto& s_lobbySearch = *reinterpret_cast<LobbySearch*>(base_address + 0x15B9D600);
	const static auto& s_infoProbe = *reinterpret_cast<InfoProbe*>(base_address + 0x1574B590);
	const static auto& window = *reinterpret_cast<HWND*>(base_address + 0x17E773D0);
	const static auto& swap_chain = reinterpret_cast<IDXGISwapChain**>(base_address + 0xF4B7858);
	const static auto& threadNames = reinterpret_cast<const char**>(base_address + 0x34393A0);
	const static auto& threadIds = reinterpret_cast<unsigned int*>(base_address + 0x175BF5D0);
	
	inline MatchMakingInfo* get_session_result(const size_t server_num)
	{
		return reinterpret_cast<MatchMakingInfo*>(base_address + 0x99AB490 + sizeof(MatchMakingInfo) * server_num);
	}
	
	inline LobbySession* get_host_session(const int lobby_type)
	{
		if (!is_valid_lobby_type(lobby_type))
		{
			return nullptr;
		}

		return reinterpret_cast<LobbySession*>(base_address + 0x1567D380 + 0x66828 * lobby_type);
	}
	
	inline LobbySession* get_client_session(const int lobby_type)
	{
		if (!is_valid_lobby_type(lobby_type))
		{
			return nullptr;
		}
		
		return reinterpret_cast<LobbySession*>(base_address + 0x15676490 + sizeof LobbySession * lobby_type);
	}

	inline clientActive_t* cl() 
	{
		return spoof_call::call(CL_GetLocalClientGlobals, 0u);
	}
	
	inline cg_t* cg() 
	{
		const auto& decrypted_cl_ptr = cl(); 
		
		if (decrypted_cl_ptr == nullptr)
			return nullptr;
		
		const auto decrypted_cg_ptr = reinterpret_cast<std::uintptr_t>(decrypted_cl_ptr) - 0x15CE0B0;
		return reinterpret_cast<cg_t*>(decrypted_cg_ptr + sizeof cg_t * 0);
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
}