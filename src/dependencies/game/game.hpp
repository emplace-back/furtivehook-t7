#pragma once
#include "dependencies/std_include.hpp"
#include "utils/string.hpp"
#include "utils/spoof_call/spoof_call.hpp"
#include "structs.hpp"
#include "offsets.hpp"

#define OFFSET(x) game::get_offset(x)

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
				int server_id;
				int message_sequence;
				int command_sequence;
				int acknowledge;
				std::string data;
			}; 
			
			extern bool writing;
			
			bool send(const NetChanMsgType type, const std::string& data, const netadr_t& netadr, const uint64_t target_xuid, const uint64_t sender_xuid = 0);
			bool send_oob(const uint64_t xuid, const netadr_t& netadr, const std::string& data, const bool fill = false);
			bool write(const write_packet& packet, const netadr_t& netadr, const uint64_t target_xuid, const uint64_t sender_xuid = 0, const bool compress_buffer = true);
			bool write(const std::string& data);
		}
		
		namespace oob
		{
			bool send(const netadr_t& target, const std::string& data);
			bool register_remote_addr(const HostInfo& info, netadr_t* addr);
			netadr_t register_remote_addr(const InfoResponseLobby& lobby);
			void on_registered_addr(const HostInfo& info, const std::function<void(const game::netadr_t&)>& callback);
			void on_registered_addr(const InfoResponseLobby& lobby, const std::function<void(const game::netadr_t&)>& callback);
		}
		
		bool send(const netadr_t& netadr, const std::string& data);
	}
	
	uintptr_t get_base();
	void initialize();
	bool in_game();
	int find_target_from_addr(const LobbySession* session, const netadr_t& from);
	bool can_connect_to_player(const LobbySession* session, const size_t client_num, const size_t target_xuid);
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
	void connect_to_session(const game::HostInfo& info);
	
	extern std::array<scr_string_t, static_cast<std::uint32_t>(bone_tag::num_tags)> bone_tags;

	inline auto get_tag(const bone_tag t)
	{
		if (t >= bone_tag::num_tags)
			return static_cast<scr_string_t>(0u);

		return bone_tags[static_cast<size_t>(t)];
	}

	inline auto get_offset(const uintptr_t val)
	{
		return get_base() + (val - 0x7FF6C52E0000);
	}

	template <typename T = void, typename... Args>
	inline auto call(const uintptr_t address, Args ... args)
	{
		return spoof_call(reinterpret_cast<T(*)(Args...)>(get_offset(address)), args...);
	}

	template <typename T = void, typename... Args>
	inline auto call(const void* address, const size_t index, Args ... args)
	{
		const auto table = *reinterpret_cast<uintptr_t**>(uintptr_t(address));
		return reinterpret_cast<T(*)(const void*, Args ...)>(table[index])(address, args...);
	}

	inline LobbySession* get_host_session(const int lobby_type)
	{
		if (!is_valid_lobby_type(lobby_type))
		{
			return nullptr;
		}

		return reinterpret_cast<LobbySession*>(OFFSET(0x7FF6DA95E380) + 0x66828ull * lobby_type);
	}

	inline LobbySession* get_client_session(const int lobby_type)
	{
		if (!is_valid_lobby_type(lobby_type))
		{
			return nullptr;
		}

		return reinterpret_cast<LobbySession*>(OFFSET(0x7FF6DA957490) + sizeof LobbySession * lobby_type);
	}

	inline LobbySession* session_data()
	{
		const auto party_session = game::get_client_session(game::LOBBY_TYPE_PRIVATE);
		const auto lobby_session = game::get_client_session(game::LOBBY_TYPE_GAME);

		return lobby_session->active ? lobby_session : party_session;
	}

	inline clientActive_t* cl()
	{
		return call<clientActive_t*>(0x7FF6C5351BD0, 0);
	}

	inline clientConnection_t* clc()
	{
		return reinterpret_cast<clientConnection_t*>(*reinterpret_cast<std::uintptr_t*>(OFFSET(0x7FF6CA6BABB8)));
	}

	inline cg_t* cg()
	{
		const auto& decrypted_cl_ptr = cl();

		if (decrypted_cl_ptr == nullptr)
			return nullptr;

		const auto decrypted_cg_ptr = uintptr_t(decrypted_cl_ptr) - 0x15CE0B0;
		return reinterpret_cast<cg_t*>(decrypted_cg_ptr);
	}

	inline centity_t* centity(const int index)
	{
		const auto& decrypted_cg_ptr = cg();

		if (decrypted_cg_ptr == nullptr)
			return nullptr;

		const auto decrypted_centity_ptr = uintptr_t(decrypted_cg_ptr) + 0x8AAAE0;
		return reinterpret_cast<centity_t*>(decrypted_centity_ptr + sizeof centity_t * index);
	}

	const static auto dwGetConnectionStatus = reinterpret_cast<bdDTLSAssociationStatus(*)(netadr_t*)>(OFFSET(0x7FF6C671CAB0));
	const static auto UI_SafeTranslateString = reinterpret_cast<const char* (*)(const char*)>(OFFSET(0x7FF6C756F7B0));
	const static auto LobbyJoinSource_IMInfoResponse = reinterpret_cast<bool(*)(const ControllerIndex_t, const uint64_t, msg_t*)>(OFFSET(0x7FF6C71C5E00));
	const static auto LobbyJoinSource_IMInfoRequest = reinterpret_cast<bool(*)(const ControllerIndex_t, const uint64_t, uint32_t)>(OFFSET(0x7FF6C71C2620));
	const static auto LobbyMsgRW_IsEndOfArray = reinterpret_cast<bool(*)(msg_t*)>(OFFSET(0x7FF6C71D6520));
	const static auto CG_SimulateBulletFire_EndPos = reinterpret_cast<void(*)(int*, float, float, const Vec3*, Vec3*, Vec3*, float, float, const Vec3*, const Vec3*, const Vec3*, float, Weapon, int, int)>(OFFSET(0x7FF6C651DB50));
	const static auto Cmd_AddCommandInternal = reinterpret_cast<void(*)(const char*, void(*)(), cmd_function_s*)>(OFFSET(0x7FF6C73CD530));
	const static auto DB_GetXAssetName = reinterpret_cast<const char* (*)(const XAsset*)>(OFFSET(0x7FF6C66C9DA0));
	const static auto BG_GetDepth = reinterpret_cast<float(*)(BulletHitInfo*, const BulletTraceResults*, const BulletFireParams*, const BulletTraceResults*)>(OFFSET(0x7FF6C5382CF0));
	const static auto BG_GetMinDamageRangeScaled = reinterpret_cast<float(*)(Weapon)>(OFFSET(0x7FF6C79D1960));
	const static auto BG_GetMultishotBaseMinDamageRangeScaled = reinterpret_cast<float(*)(Weapon)>(OFFSET(0x7FF6C79D1550));
	const static auto BG_GetSpreadForWeapon = reinterpret_cast<void(*)(playerState_s const*, const Weapon, float*, float*)>(OFFSET(0x7FF6C79B1310));
	const static auto BG_seedRandWithGameTime = reinterpret_cast<int(*)(int*)>(OFFSET(0x7FF6C7968670));
	const static auto AngleVectors = reinterpret_cast<void(*)(const Vec3*, Vec3*, Vec3*, Vec3*)>(OFFSET(0x7FF6C758B9E0));
	const static auto ClampChar = reinterpret_cast<char(*)(const int i)>(OFFSET(0x7FF6C75862B0));
	const static auto CG_ClientHasPerk = reinterpret_cast<bool(*)(LocalClientNum_t, ClientNum_t, unsigned int)>(OFFSET(0x7FF6C5C235D0));
	const static auto BG_GetWeaponDef = reinterpret_cast<WeaponDef * (*)(Weapon)>(OFFSET(0x7FF6C79CAB80));
	const static auto BG_GetPenetrateType = reinterpret_cast<PenetrateType(*)(const Weapon)>(OFFSET(0x7FF6C79D1ED0));
	const static auto BG_GetSurfacePenetrationDepth = reinterpret_cast<float(*)(PenetrateType penetrateType, int surfaceType)>(OFFSET(0x7FF6C79B1480));
	const static auto BG_AdvanceTrace = reinterpret_cast<bool(*)(BulletFireParams * bp, BulletTraceResults*, float)>(OFFSET(0x7FF6C5382020));
	const static auto BG_IsDualWield = reinterpret_cast<bool(*)(const Weapon)>(OFFSET(0x7FF6C79D4820));
	const static auto BG_GetViewmodelWeaponIndex = reinterpret_cast<Weapon(*)(const playerState_s*)>(OFFSET(0x7FF6C79B14A0));
	const static auto BG_GetWeaponDamageForRange = reinterpret_cast<int(*)(const Weapon, const Vec3*, const Vec3*)>(OFFSET(0x7FF6C79D3800));
	const static auto BG_GetWeaponHitLocationMultiplier = reinterpret_cast<float(*)(const Weapon, uint16_t)>(OFFSET(0x7FF6C79D3B80));
	const static auto G_GetWeaponHitLocationMultiplier = reinterpret_cast<float(*)(uint16_t, Weapon)>(OFFSET(0x7FF6C6C673C0));
	const static auto Trace_GetEntityHitId = reinterpret_cast<std::uint16_t(__fastcall*)(const trace_t*)>(OFFSET(0x7FF6C73C5AD0));
	const static auto vectoangles = reinterpret_cast<void(*)(const Vec3*, Vec3*)>(OFFSET(0x7FF6C758B710));
	const static auto Com_GetClientDObj = reinterpret_cast<void* (__fastcall*)(int, LocalClientNum_t)>(OFFSET(0x7FF6C742EBA0));
	const static auto GScr_AllocString = reinterpret_cast<scr_string_t(__fastcall*)(const char*)>(OFFSET(0x7FF6C6D63520));
	const static auto CG_DObjGetWorldTagPos = reinterpret_cast<bool(__fastcall*)(const cpose_t*, void*, scr_string_t, Vec3*)>(OFFSET(0x7FF6C548BD50));
	const static auto LobbyClientLaunch_IsInGame = *reinterpret_cast<bool(__fastcall*)()>(OFFSET(0x7FF6C71AE6D0));
	const static auto dwRegisterSecIDAndKey = reinterpret_cast<bool(*)(const bdSecurityID*, const bdSecurityKey*)>(OFFSET(0x7FF6C671E120));
	const static auto dwCommonAddrToNetadr = reinterpret_cast<bool(*)(netadr_t* const, const void* const, const bdSecurityID*)>(OFFSET(0x7FF6C671C360));
	const static auto MSG_InfoResponse = reinterpret_cast<bool(__fastcall*)(void*, msg_t*)>(OFFSET(0x7FF6C71C1EB0));
	const static auto LobbyMsgRW_PackageShort = reinterpret_cast<bool(*)(game::msg_t*, const char*, short*)>(OFFSET(0x7FF6C71D67A0));
	const static auto LobbyMsgRW_PackageBool = reinterpret_cast<bool(*)(game::msg_t*, const char*, bool*)>(OFFSET(0x7FF6C71D65D0));
	const static auto LobbyMsgRW_PackageString = reinterpret_cast<bool(*)(game::msg_t*, const char*, char*, int)>(OFFSET(0x7FF6C71D67C0));
	const static auto LobbyMsgRW_PackageUShort = reinterpret_cast<bool(__fastcall*)(msg_t*, const char*, std::uint16_t*)>(OFFSET(0x7FF6C71D6970));
	const static auto LobbyMsgRW_PackageUChar = reinterpret_cast<bool(__fastcall*)(msg_t*, const char*, std::uint8_t*)>(OFFSET(0x7FF6C71D6850));
	const static auto LobbyMsgRW_PackageUInt64 = reinterpret_cast<bool(__fastcall*)(msg_t*, const char*, std::uint64_t*)>(OFFSET(0x7FF6C71D6870));
	const static auto LobbyMsgRW_PackageUInt = reinterpret_cast<bool(__fastcall*)(msg_t*, const char*, std::uint32_t*)>(OFFSET(0x7FF6C71D68F0));
	const static auto LobbyMsgRW_PackageXuid = reinterpret_cast<bool(__fastcall*)(msg_t*, const char*, std::uint64_t*)>(OFFSET(0x7FF6C71D6990));
	const static auto LobbyMsgRW_PackageArrayStart = reinterpret_cast<bool(__fastcall*)(msg_t*, const char*)>(OFFSET(0x7FF6C71D6560));
	const static auto LobbyMsgRW_PackageInt = reinterpret_cast<bool(__fastcall*)(msg_t*, const char*, int*)>(OFFSET(0x7FF6C71D6720));
	const static auto LobbyMsgRW_PackageGlob = reinterpret_cast<bool(__fastcall*)(msg_t*, const char*, void*, int)>(OFFSET(0x7FF6C71D6700));
	const static auto LobbyMsgRW_PrepReadMsg = reinterpret_cast<bool(__fastcall*)(msg_t*, char*, int)>(OFFSET(0x7FF6C71D6A10));
	const static auto LiveUser_IsXUIDLocalPlayer = reinterpret_cast<bool(__fastcall*)(const std::uint64_t)>(OFFSET(0x7FF6C71A71C0));
	const static auto dwNetadrToCommonAddr = reinterpret_cast<bool(__fastcall*)(netadr_t, void*, const unsigned int, bdSecurityID*)>(OFFSET(0x7FF6C671DA60));
	const static auto Sys_GetTLS = reinterpret_cast<TLSData * (__fastcall*)()>(OFFSET(0x7FF6C7464210));
	const static auto MSG_ReadData = reinterpret_cast<void(*)(msg_t*, void*, int)>(OFFSET(0x7FF6C7435F10));
	const static auto MSG_Init = reinterpret_cast<void(*)(msg_t*, char*, int)>(OFFSET(0x7FF6C74359E0));
	const static auto MSG_InitReadOnly = reinterpret_cast<void(*)(msg_t*, std::uint8_t*, int)>(OFFSET(0x7FF6C7435A70));
	const static auto MSG_BeginReading = reinterpret_cast<void(*)(msg_t*)>(OFFSET(0x7FF6C7435760));
	const static auto LiveUser_GetXuid = reinterpret_cast<std::uint64_t(*)(const ControllerIndex_t)>(OFFSET(0x7FF6C71A73A0));
	const static auto CL_AddReliableCommand = reinterpret_cast<bool(*)(LocalClientNum_t, const char*)>(OFFSET(0x7FF6C663D4C0));
	const static auto Cmd_ExecuteSingleCommand = reinterpret_cast<void(__fastcall*)(LocalClientNum_t, ControllerIndex_t, const char*, bool)>(OFFSET(0x7FF6C73CDC20));
	const static auto Cbuf_AddText = reinterpret_cast<void(__fastcall*)(LocalClientNum_t, const char*)>(OFFSET(0x7FF6C73CC8B0));
	const static auto dwInstantSendMessage = reinterpret_cast<bool(*)(ControllerIndex_t, const std::uint64_t*, unsigned int, char, const void*, unsigned int)>(OFFSET(0x7FF6C671A810));
	const static auto Live_IsUserSignedInToDemonware = reinterpret_cast<bool(*)(const ControllerIndex_t)>(OFFSET(0x7FF6C70ED830));
	const static auto CL_GetClientName = reinterpret_cast<bool(*)(LocalClientNum_t, int, char*, int, bool)>(OFFSET(0x7FF6C66C3140));
	const static auto NET_CompareAdr = reinterpret_cast<bool(*)(const netadr_t, const netadr_t)>(OFFSET(0x7FF6C74532D0));
	const static auto LivePresence_Serialize = reinterpret_cast<int(*)(bool, PresenceData*, const void*, size_t)>(OFFSET(0x7FF6C7173D80));
	const static auto task_livepresence_dw_get = reinterpret_cast<game::TaskDefinition*>(OFFSET(0x7FF6C82F9A40));
	const static auto TaskManager2_TaskGetInProgress = reinterpret_cast<bool(*)(const void*)>(OFFSET(0x7FF6C7591780));
	const static auto TaskManager2_SetupNestedTask = reinterpret_cast<TaskRecord * (*)(const void*, ControllerIndex_t, TaskRecord*, void*)>(OFFSET(0x7FF6C7591380));
	const static auto dwPresenceGet = reinterpret_cast<TaskRecord * (*)(const ControllerIndex_t, dwPresenceTask* const)>(OFFSET(0x7FF6C671E7B0));
	const static auto TaskManager2_CreateTask = reinterpret_cast<TaskRecord * (*)(const void*, const ControllerIndex_t, TaskRecord*, int)>(OFFSET(0x7FF6C7590770));
	const static auto TaskManager2_StartTask = reinterpret_cast<void(*)(TaskRecord*)>(OFFSET(0x7FF6C75916C0));
	const static auto& g_fHitLocDamageMult = reinterpret_cast<float*>(OFFSET(0x7FF6CF36D4D0));
	const static auto& g_assetNames = reinterpret_cast<const char**>(OFFSET(0x7FF6C860A080));
	const static auto& s_infoProbe = reinterpret_cast<InfoProbe*>(OFFSET(0x7FF6DAA2C590));
	const static auto& window = *reinterpret_cast<HWND*>(OFFSET(0x7FF6DD1583D0));
	const static auto& lobbyMsgName = reinterpret_cast<const char**>(OFFSET(0x7FF6C86F14A0));
	const static auto& s_presenceTaskData = reinterpret_cast<dwPresenceTask*>(OFFSET(0x7FF6D66F00A8));
}
