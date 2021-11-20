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
		void send_connection_test(const netadr_t& to, const size_t client_num);
		void send_lobby_msg(const netadr_t& to, const LobbyMsg* lobby_msg, const LobbyModule module);
		bool send(const game::netadr_t & target, const std::string & data, const game::netsrc_t & sock = game::NS_SERVER);
		bool register_remote_addr(const game::XSESSION_INFO * info, netadr_t * addr);
	}
	
	void initialize();
	bool send_unhandled_netchan_message(const game::netadr_t & from, const game::LobbyMsg & lobby_msg, const std::uint64_t sender_id);
	bool send_unhandled_message(const netadr_t & target, const std::string & command);
	std::string get_oob_command(const game::netadr_t & target);
	void bold_game_message(const char* msg, ...);
	game::netadr_t LobbySession_GetNetAddr(const game::SessionClient * sessionClient);
	bool NET_CompareXNAddr(const XSESSION_INFO * a, const XSESSION_INFO * b);
	bool NET_CompareXNAddr(const XNADDR * a, const XNADDR * b);
	int find_target_from_addr(const netadr_t & from);
	int Party_FindMemberByXUID(const std::uint64_t player);
	bool can_connect_to_player(const size_t client_num, const size_t target_xuid);
	game::XSESSION_INFO get_session_info(const game::InfoResponseLobby & lobby);
	void on_every_frame();
	bool is_enemy(const int client_num);
	Vec2 get_screen_pos(const Vec3 & world_pos);
	Vec2 get_scale(const centity_t * cent, const float pos);
	Vec3 get_top_position(const centity_t * cent);
	bool AimTarget_GetTagPos(const game::centity_t * cent, const scr_string_t & tag_name, Vec3 * end);
	bool CG_GetPlayerViewOrigin(const game::playerState_s* ps, Vec3* view_origin);
	void adjust_user_cmd_movement(usercmd_s * cmd_old, const float angle, const float old_angle);
	float get_weapon_damage_range(const Weapon & weapon);
	bool CG_BulletTrace(game::BulletTraceResults * br, game::BulletFireParams * bp, const int attacker_entity_num, int lastSurfaceType);
	bool is_valid_lobby_type(LobbyMsg* msg);
	bool is_valid_lobby_type(const int lobby_type);
	bool MSG_JoinMemberInfo(Msg_JoinMemberInfo * thisptr, game::LobbyMsg * msg);
	bool MSG_LobbyHostHeartbeat(Msg_LobbyHostHeartbeat * thisptr, game::LobbyMsg * msg);
	connstate_t CL_GetLocalClientConnectionState();
	bool in_game();
	bool is_valid_target(const int client_num);
	
	extern std::unordered_map<std::string, bool> handlers;
	extern std::array<scr_string_t, static_cast<std::uint32_t>(bone_tag::num_tags)> bone_tags;
	extern LobbySession* session;
	extern dvar_t* com_smoothframes_original;
	extern dvar_t* r_fxShadows_original;
	extern dvar_t* unknown_original;

	const static auto base_address = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr)) + 0x1000;
	const static auto& spoof_t = reinterpret_cast<void*>(game::base_address + 0x2A95FFB);

	const static auto ClientContentType_GetInfo = reinterpret_cast<ClientContentTypeInfo*(*)(ClientContentType)>(game::base_address + 0x1EEA780); 
	const static auto ClientContentType_GetContentTypeFromDatamask = reinterpret_cast<ClientContentType(*)(unsigned int)>(game::base_address + 0x1EEA710);
	const static auto MSG_LobbyState = reinterpret_cast<bool(__fastcall *)(void*, game::LobbyMsg*)>(game::base_address + 0x1ED4320);
	const static auto MSG_JoinAgreementRequest = reinterpret_cast<bool(__fastcall *)(void*, game::LobbyMsg*)>(game::base_address + 0x1EE0F90);
	const static auto MSG_ClientReliableData = reinterpret_cast<bool(__fastcall *)(void*, game::LobbyMsg*)>(game::base_address + 0x1ED3EF0);
	const static auto MSG_ClientContent = reinterpret_cast<bool(__fastcall *)(void*, game::LobbyMsg*)>(game::base_address + 0x1ED3DF0);
	const static auto MSG_JoinParty = reinterpret_cast<bool(__fastcall *)(void*, game::LobbyMsg*)>(game::base_address + 0x1EE1110);
	const static auto BG_GetDepth = reinterpret_cast<float(*)(game::BulletHitInfo*, const game::BulletTraceResults*, const game::BulletFireParams*, const game::BulletTraceResults*)>(game::base_address + 0xA1CF0);
	const static auto BG_GetMinDamageRangeScaled = reinterpret_cast<float(*)(game::Weapon)>(game::base_address + 0x26F0960);
	const static auto BG_GetMultishotBaseMinDamageRangeScaled = reinterpret_cast<float(*)(game::Weapon)>(game::base_address + 0x26F0550);
	const static auto sv_bullet_range = *reinterpret_cast<game::dvar_t**>(game::base_address + 0x176F96C0);
	const static auto BG_GetSpreadForWeapon = reinterpret_cast<void(*)(game::playerState_s*, const game::Weapon, float*, float*)>(game::base_address + 0x26D0310);
	const static auto BG_seedRandWithGameTime = reinterpret_cast<void(*)(unsigned int*)>(game::base_address + 0x2687670);
	const static auto AngleVectors = reinterpret_cast<void(*)(const Vec3 *, Vec3 *, Vec3 *, Vec3 *)>(game::base_address + 0x22AA9E0); 
	const static auto ClampChar = reinterpret_cast<char(*)(const int i)>(game::base_address + 0x22A52B0);
	const static auto sv_penetrationCount = *reinterpret_cast<game::dvar_t**>(game::base_address + 0x176F96A8);
	const static auto bg_bulletPenetrationTreatVoidsAsSolid = *reinterpret_cast<game::dvar_t**>(game::base_address + 0x19C2BA30);
	const static auto CG_ClientHasPerk = reinterpret_cast<bool(*)(LocalClientNum_t, ClientNum_t, unsigned int)>(game::base_address + 0x9425D0);
	const static auto BG_GetWeaponDef = reinterpret_cast<WeaponDef*(*)(game::Weapon)>(game::base_address + 0x26E9B80);
	const static auto BG_GetPenetrateType = reinterpret_cast<PenetrateType(*)(const game::Weapon)>(game::base_address + 0x26F0ED0);
	const static auto BG_FixupHitInfo = reinterpret_cast<bool(*)(game::BulletPenetrationInfo*, int, int, int, int, float *, float, PenetrateType)>(game::base_address + 0xA1B30);
	const static auto BG_GetSurfacePenetrationDepth = reinterpret_cast<float(*)(PenetrateType penetrateType, int surfaceType)>(game::base_address + 0x26D0480);
	const static auto BG_AdvanceTrace = reinterpret_cast<bool(*)(game::BulletFireParams* bp, game::BulletTraceResults*, float)>(game::base_address + 0xA1020); 
	const static auto BG_IsDualWield = reinterpret_cast<bool(*)(const Weapon)>(game::base_address + 0x26F3820);
	const static auto BG_GetViewmodelWeaponIndex = reinterpret_cast<Weapon(*)(const game::playerState_s*)>(game::base_address + 0x26D04A0);
	const static auto BG_GetWeaponDamageForRange = reinterpret_cast<int(*)(const Weapon, const Vec3*, const Vec3*)>(game::base_address + 0x26F27A0);
	const static auto G_GetWeaponHitLocationMultiplier = reinterpret_cast<float(*)(int, Weapon)>(game::base_address + 0x19863C0); 
	const static auto Trace_GetEntityHitId = reinterpret_cast<std::uint16_t(__fastcall*)(const trace_t*)>(game::base_address + 0x20E4AD0);
	const static auto vectoangles = reinterpret_cast<void(*)(const Vec3*, Vec3*)>(base_address + 0x22AA710);
	const static auto CG_WorldPosToScreenPos = reinterpret_cast<bool(__fastcall*)(LocalClientNum_t, const Vec3* worldPos, Vec2* outScreenPos)>(base_address + 0x573140);
	const static auto Com_GetClientDObj = reinterpret_cast<void*(__fastcall*)(int, LocalClientNum_t)>(base_address + 0x214DBA0);
	const static auto GScr_AllocString = reinterpret_cast<scr_string_t(__fastcall*)(const char*)>(base_address + 0x1A82520);
	const static auto CG_DObjGetWorldTagPos = reinterpret_cast<bool(__fastcall*)(const cpose_t *, void*, scr_string_t, Vec3*)>(base_address + 0x1AAD50);
	const static auto LobbyLaunch_IsInGame = *reinterpret_cast<bool(__fastcall*)(LobbySession*)>(game::base_address + 0x1EE9EA0);
	const static auto LobbyHostLaunch_IsInGame = *reinterpret_cast<bool(__fastcall*)()>(game::base_address + 0x1EDCD20);
	const static auto LobbyClientLaunch_IsInGame = *reinterpret_cast<bool(__fastcall*)()>(game::base_address + 0x1ECD6D0);
	const static auto SendClientReliableData = reinterpret_cast<bool(__fastcall *)(const ControllerIndex_t, int, Msg_ClientReliableData *)>(game::base_address + 0x1ED5350);
	const static auto dwRegisterSecIDAndKey = reinterpret_cast<bool(*)(const bdSecurityID *, const bdSecurityKey *)>(base_address + 0x143D120);
	const static auto dwCommonAddrToNetadr = reinterpret_cast<bool(*)(netadr_t *const, const void *const, const bdSecurityID *)>(base_address + 0x143B360);
	const static auto LobbyMsg_SendInfoRequest = reinterpret_cast<bool(__fastcall*)(const ControllerIndex_t, const std::uint64_t*, int, const game::Msg_InfoRequest *)>(game::base_address + 0x1EE44C0);
	const static auto MSG_InfoResponse = reinterpret_cast<bool(__fastcall *)(void*, LobbyMsg *)>(base_address + 0x1EE0EB0);
	const static auto LobbyMsgRW_PackageUShort = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, const char*, const std::uint16_t*)>(base_address + 0x1EF5970); 
	const static auto LobbyMsgRW_PackageUChar = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, const char*, const std::uint8_t*)>(base_address + 0x1EF5850);
	const static auto LobbyMsgRW_PrepWriteMsg = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, char *, int, MsgType)>(base_address + 0x1EF5A80);
	const static auto LobbyMsgRW_PackageUInt64 = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, const char *, const std::uint64_t*)>(base_address + 0x1EF5870);
	const static auto LobbyMsgRW_PackageUInt = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, const char *, const std::uint32_t*)>(base_address + 0x1EF58F0);
	const static auto LobbyMsgRW_PackageXuid = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, const char*, const std::uint64_t*)>(base_address + 0x1EF5990);
	const static auto LobbyMsgRW_PackageArrayStart = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, const char*)>(base_address + 0x1EF5560);
	const static auto LobbyMsgRW_PackageElement = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, bool)>(base_address + 0x1EF5610); 
	const static auto LobbyMsgRW_PackageInt = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, const char*, const int*)>(base_address + 0x1EF5720);
	const static auto LobbyMsgRW_PackageGlob = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, const char*, void*, int)>(base_address + 0x1EF5700);
	const static auto LobbyMsgRW_PrepReadMsg = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, msg_t*)>(base_address + 0x1EF5A40);
	const static auto LobbyMsgRW_PrepReadData = reinterpret_cast<bool(__fastcall *)(LobbyMsg *, char *, int)>(base_address + 0x1EF5A10);
	const static auto LobbyMsg_SendMsg_ConnectionTest = reinterpret_cast<bool(__fastcall *)(const ControllerIndex_t, LobbySession*, Msg_ConnectionTest *,std::uint64_t)>(game::base_address + 0x1EF8350);
	const static auto LobbySession_GetClientNumByXuid = reinterpret_cast<int(__fastcall*)(LobbyType, std::uint64_t)>(game::base_address + 0x1ECB0C0);
	const static auto LiveUser_IsXUIDLocalPlayer = reinterpret_cast<bool(__fastcall*)(const std::uint64_t)>(game::base_address + 0x1EC61C0);
	const static auto LobbyClientMsg_SendDisconnectClient = reinterpret_cast<bool(__fastcall*)(const ControllerIndex_t, LobbyType, std::uint64_t, LobbyDisconnectClient)>(game::base_address + 0x1ED30E0);
	const static auto LobbyClientMsg_SendDisconnect = reinterpret_cast<bool(__fastcall*)(const ControllerIndex_t, LobbyType, std::uint64_t)>(game::base_address + 0x1ED3050);
	const static auto dwNetadrToCommonAddr = reinterpret_cast<bool(__fastcall*)(game::netadr_t, void*, const unsigned int, game::bdSecurityID*)>(base_address + 0x143CA60);
	const static auto Com_Error = reinterpret_cast<void(__fastcall*)(const char*, int, game::errorCode, const char*)>(base_address + 0x20F7BD0);
	const static auto Sys_GetTLS = reinterpret_cast<game::TLSData*(__fastcall*)()>(base_address + 0x2183210);
	const static auto LobbyTypes_GetLobbyTypeName = reinterpret_cast<const char*(__fastcall*)(const LobbyType)>(base_address + 0x1EEACE0);
	const static auto LobbyTypes_GetMsgTypeName = reinterpret_cast<const char*(__fastcall*)(const MsgType)>(base_address + 0x1EEAD60);
	const static auto MSG_ReadData = reinterpret_cast<void(*)(game::msg_t*, void*, int)>(base_address + 0x2154F10); 
	const static auto MSG_Init = reinterpret_cast<void(*)(game::msg_t*, char*, int)>(base_address + 0x21549E0);
	const static auto MSG_InitReadOnly = reinterpret_cast<void(*)(game::msg_t*, const char *, int)>(base_address + 0x2154A70);
	const static auto MSG_BeginReading = reinterpret_cast<void(*)(game::msg_t*)>(base_address + 0x2154760);
	const static auto MSG_ReadByte = reinterpret_cast<std::uint8_t(*)(game::msg_t *)>(base_address + 0x2154EB0);
	const static auto MSG_WriteString = reinterpret_cast<void(*)(game::msg_t*, const char*)>(base_address + 0x2157C80);
	const static auto MSG_WriteShort = reinterpret_cast<void(*)(game::msg_t*, int)>(base_address + 0x2172130);
	const static auto MSG_WriteByte = reinterpret_cast<void(*)(game::msg_t*, int)>(base_address + 0x2157220);
	const static auto MSG_WriteData = reinterpret_cast<void(*)(game::msg_t*, const void *, int)>(base_address + 0x2157240);
	const static auto CG_BoldGameMessageCenter = reinterpret_cast<void(__fastcall*)(LocalClientNum_t, const char *, int)>(base_address + 0x8C3C80);
	const static auto LiveUser_GetClientName = reinterpret_cast<const char*(*)(const ControllerIndex_t)>(base_address + 0x1EC6370);
	const static auto LiveUser_GetXuid = reinterpret_cast<std::uint64_t(*)(const ControllerIndex_t)>(base_address + 0x1EC63A0);
	const static auto CL_AddReliableCommand = reinterpret_cast<bool(__fastcall*)(LocalClientNum_t, const char*)>(base_address + 0x135C4C0);
	const static auto Cbuf_AddText = reinterpret_cast<void(__fastcall*)(LocalClientNum_t, const char*)>(base_address + 0x20EB8B0);
	const static auto LobbyClientMsg_SendModifiedStats = reinterpret_cast<bool(__fastcall*)(const ControllerIndex_t, const LobbyType)>(base_address + 0x1ED32A0);
	const static auto LiveSteam_PopOverlayForSteamID = reinterpret_cast<void(*)(std::uint64_t)>(base_address + 0x1EB0010);
	const static auto dwInstantSendMessage = reinterpret_cast<void*(__fastcall*)(ControllerIndex_t, const std::uint64_t*, std::uint32_t, std::uint8_t, const void *, unsigned int)>(base_address + 0x1439810);
	const static auto NET_OutOfBandPrint = reinterpret_cast<bool(__fastcall*)(netsrc_t, netadr_t, const char *)>(base_address + 0x2173170);
	const static auto NET_OutOfBandData = reinterpret_cast<bool(__fastcall*)(netsrc_t, netadr_t, const char *, int)>(base_address + 0x2173060);
	const static auto Live_IsUserSignedInToDemonware = reinterpret_cast<bool(__fastcall*)(const ControllerIndex_t)>(base_address + 0x1E0C830);
	const static auto Com_IsInGame = reinterpret_cast<bool(__fastcall*)()>(base_address + 0x2147D20);
	const static auto Com_IsRunningUILevel = reinterpret_cast<bool(__fastcall*)()>(base_address + 0x2147DB0);
	const static auto CL_GetClientName = reinterpret_cast<bool(__fastcall*)(LocalClientNum_t, int, char*, int, bool)>(base_address + 0x13E2140);
	const static auto NET_CompareAdr = reinterpret_cast<bool(__fastcall*)(const netadr_t, const netadr_t)>(base_address + 0x21722D0);
	const static auto LiveSteam_GetFriends = reinterpret_cast<ISteamFriends*(__fastcall*)(std::uintptr_t)>(base_address + 0x1DF99F0);
	const static auto CL_GetLocalClientGlobals = *reinterpret_cast<clientActive_t*(*)(LocalClientNum_t)>(game::base_address + 0x70BD0);
	const static auto CG_GetPredictedPlayerState = *reinterpret_cast<playerState_s*(*)(LocalClientNum_t)>(game::base_address + 0x8DE6A0);
	const static auto LobbySession_GetSession_Internal = *reinterpret_cast<LobbySession*(*)(LobbyType)>(game::base_address + 0x1ECCAB0);
	const static auto lobbymsg_prints = reinterpret_cast<game::dvar_t**>(game::base_address + 0x1574E840);
	const static auto& window = *reinterpret_cast<HWND*>(base_address + 0x17E773D0);
	const static auto& swap_chain = reinterpret_cast<IDXGISwapChain**>(base_address + 0xF4B7858);
	
	inline LobbySession* LobbySession_GetSession(const LobbyType lobbyType) {
		return reinterpret_cast<LobbySession*>(base_address + 0x15676490 + sizeof LobbySession * lobbyType);
	}

	inline clientActive_t* cl() {
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

	inline centity_t* centity(const int index) {
		const auto& decrypted_cg_ptr = cg();
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