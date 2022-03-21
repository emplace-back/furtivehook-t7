#pragma once
#include "dependencies/std_include.hpp"

#define DOT_PRODUCT(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define ANGLE2SHORT(a) ((int)((a)*(65536/360.0f))&65535)
#define SHORT2ANGLE(a) ((float)((a)*(360.0f/65536)))
#define M_PI 3.14159265358979323846
#define M_PI_DOUBLE ((float)M_PI*2.0f)
#define DEG2RAD(a) ((a)*((float)M_PI/180.0f))
#define RAD2DEG(a) ((a)*(180.0f/(float)M_PI))
#define GET_SIGN(a) ((a) ? ((*(int*)&(a) >> 31) | 1) : 0)
#define VECTOR_COPY(b, a) ((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])

class Vec2 {
public:
	float x, y;

	Vec2() :x(0), y(0) {}
	Vec2(const float x, const float y) : x(x), y(y) {}
	Vec2(const Vec2& v) : x(v.x), y(v.y) {}
	Vec2(const float fl[2]) : x(fl[0]), y(fl[1]) {}

	operator float*() { return &x; }
	operator const float*() const { return &x; }

	bool operator==(const Vec2& v) const { return x == v.x && y == v.y; };
	bool operator!=(const Vec2& v) const { return !(*this == v); };

	Vec2 operator-() const { return Vec2(-x, -y); }
	Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
	Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
	Vec2 operator*(const float fl) const { return Vec2(x * fl, y * fl); }
	Vec2 operator/(const float fl) const { return Vec2(x / fl, y / fl); }

	void operator+=(const Vec2& v) { x += v.x; y += v.y; }
	void operator-=(const Vec2& v) { x -= v.x; y -= v.y; }
	void operator*=(const Vec2& v) { x *= v.x; y *= v.y; }
	void operator*=(const float fl) { x *= fl; y *= fl; }
	void operator/=(const Vec2& v) { x /= v.x; x /= v.y; }

	void set(const float x, const float y) {
		this->x = x;
		this->y = y;
	}

	void rotate(const double deg) {
		const double theta = deg / 180.0 * M_PI;
		double c = cos(theta);
		double s = sin(theta);
		double tx = x * c - y * s;
		double ty = x * s + y * c;
		x = tx;
		y = ty;
	}

	void clear() {
		this->x = 0.0f; this->y = 0.0f;
	}

	bool empty() const {
		return this->x == 0.0f && this->y == 0.0f;
	}

	Vec2& normalize() {
		if (length() == 0) return *this;
		*this *= (1.0f / length());
		return *this;
	}

	float distance(const Vec2& v) const {
		Vec2 d(x - v.x, y - v.y);
		return d.length();
	}

	float length() const {
		return std::sqrtf(x * x + y * y);
	}
};

class Vec3
{
public:
	float x, y, z;

	Vec3() :x(0), y(0), z(0) {}
	Vec3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
	Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
	Vec3(const float fl[3]) : x(fl[0]), y(fl[1]), z(fl[3]) {}

	operator float* () { return &x; }
	operator const float* () const { return &x; }

	bool operator==(const Vec3& v) const { return x == v.x && y == v.y && z == v.z; }
	bool operator!=(const Vec3& v) const { return !(*this == v); }

	Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
	Vec3 operator-() const { return Vec3(-x, -y, -z); }
	Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
	Vec3 operator*(const float fl) const { return Vec3(x * fl, y * fl, z * fl); }
	Vec3 operator/(const float fl) const { return Vec3(x / fl, y / fl, z / fl); }

	void operator += (const Vec3& add) { x += add.x; y += add.y; z += add.z; }
	void operator -= (const Vec3& sub) { x -= sub.x; y -= sub.y; z -= sub.z; }
	void operator *= (const float mul) { x *= mul; y *= mul; z *= mul; }
	void operator /= (const float mul) { x /= mul; y /= mul; z /= mul; }

	void set(const float value) {
		x = value; y = value; z = value;
	}

	void clear() 
	{
		this->set(0.0f);
	}

	float length() const {
		return std::sqrtf(x * x + y * y + z * z);
	}

	bool empty() const {
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	float dot() const {
		return (x * x + y * y + z * z);
	}

	Vec3 cross_product(const Vec3& other) const {
		return { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };
	}

	Vec3& normalized() {
		if (length() == 0) return *this;
		*this *= (1.0 / length());
		return *this;
	}

	float distance(Vec3 v) const {
		Vec3 d(x - v.x, y - v.y, z - v.z);
		return d.length();
	}
};

using LocalClientNum_t = std::uint32_t;
using scr_string_t = std::uint32_t;
using ControllerIndex_t = std::uint32_t;
using ClientNum_t = std::uint32_t;

namespace game
{
	typedef uint32_t dvarStrHash_t;
	
	struct $0892FF7CFA16307BC31F976BA8D88FE7
	{
		unsigned __int64 weaponIdx : 9;
		unsigned __int64 attachment1 : 6;
		unsigned __int64 attachment2 : 6;
		unsigned __int64 attachment3 : 6;
		unsigned __int64 attachment4 : 6;
		unsigned __int64 attachment5 : 6;
		unsigned __int64 attachment6 : 6;
		unsigned __int64 attachment7 : 6;
		unsigned __int64 attachment8 : 6;
		unsigned __int64 padding : 7;
	};
	
	union Weapon
	{
		$0892FF7CFA16307BC31F976BA8D88FE7 _anon_0;
		uint64_t weaponData;
	};
	
	enum XAssetType
	{
		ASSET_TYPE_PHYSPRESET = 0x0,
		ASSET_TYPE_PHYSCONSTRAINTS = 0x1,
		ASSET_TYPE_DESTRUCTIBLEDEF = 0x2,
		ASSET_TYPE_XANIMPARTS = 0x3,
		ASSET_TYPE_XMODEL = 0x4,
		ASSET_TYPE_XMODELMESH = 0x5,
		ASSET_TYPE_MATERIAL = 0x6,
		ASSET_TYPE_COMPUTE_SHADER_SET = 0x7,
		ASSET_TYPE_TECHNIQUE_SET = 0x8,
		ASSET_TYPE_IMAGE = 0x9,
		ASSET_TYPE_SOUND = 0xA,
		ASSET_TYPE_CLIPMAP = 0xB,
		ASSET_TYPE_COMWORLD = 0xC,
		ASSET_TYPE_GAMEWORLD = 0xD,
		ASSET_TYPE_GFXWORLD = 0xE,
		ASSET_TYPE_FONTICON = 0xF,
		ASSET_TYPE_LOCALIZE_ENTRY = 0x10,
		ASSET_TYPE_LOCALIZE_LIST = 0x11,
		ASSET_TYPE_WEAPON = 0x12,
		ASSET_TYPE_WEAPON_FULL = 0x13,
		ASSET_TYPE_CGMEDIA = 0x14,
		ASSET_TYPE_PLAYERSOUNDS = 0x15,
		ASSET_TYPE_PLAYERFX = 0x16,
		ASSET_TYPE_SHAREDWEAPONSOUNDS = 0x17,
		ASSET_TYPE_ATTACHMENT = 0x18,
		ASSET_TYPE_ATTACHMENT_UNIQUE = 0x19,
		ASSET_TYPE_WEAPON_CAMO = 0x1A,
		ASSET_TYPE_CUSTOMIZATION_TABLE = 0x1B,
		ASSET_TYPE_CUSTOMIZATION_TABLE_FE_IMAGES = 0x1C,
		ASSET_TYPE_CUSTOMIZATION_TABLE_COLOR = 0x1D,
		ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1E,
		ASSET_TYPE_FX = 0x1F,
		ASSET_TYPE_TAGFX = 0x20,
		ASSET_TYPE_NEW_LENSFLARE_DEF = 0x21,
		ASSET_TYPE_IMPACT_FX = 0x22,
		ASSET_TYPE_IMPACT_SOUND = 0x23,
		ASSET_TYPE_AITYPE = 0x24,
		ASSET_TYPE_CHARACTER = 0x25,
		ASSET_TYPE_XMODELALIAS = 0x26,
		ASSET_TYPE_RAWFILE = 0x27,
		ASSET_TYPE_STRINGTABLE = 0x28,
		ASSET_TYPE_STRUCTURED_TABLE = 0x29,
		ASSET_TYPE_LEADERBOARD = 0x2A,
		ASSET_TYPE_DDL = 0x2B,
		ASSET_TYPE_GLASSES = 0x2C,
		ASSET_TYPE_SCRIPTPARSETREE = 0x2D,
		ASSET_TYPE_SCRIPTPARSETREEDBG = 0x2E,
		ASSET_TYPE_KEYVALUEPAIRS = 0x2F,
		ASSET_TYPE_VEHICLEDEF = 0x30,
		ASSET_TYPE_TRACER = 0x31,
		ASSET_TYPE_SURFACEFX_TABLE = 0x32,
		ASSET_TYPE_SURFACESOUNDDEF = 0x33,
		ASSET_TYPE_FOOTSTEP_TABLE = 0x34,
		ASSET_TYPE_ENTITYFXIMPACTS = 0x35,
		ASSET_TYPE_ENTITYSOUNDIMPACTS = 0x36,
		ASSET_TYPE_ZBARRIER = 0x37,
		ASSET_TYPE_VEHICLEFXDEF = 0x38,
		ASSET_TYPE_VEHICLESOUNDDEF = 0x39,
		ASSET_TYPE_TYPEINFO = 0x3A,
		ASSET_TYPE_SCRIPTBUNDLE = 0x3B,
		ASSET_TYPE_SCRIPTBUNDLELIST = 0x3C,
		ASSET_TYPE_RUMBLE = 0x3D,
		ASSET_TYPE_BULLETPENETRATION = 0x3E,
		ASSET_TYPE_LOCDMGTABLE = 0x3F,
		ASSET_TYPE_AIMTABLE = 0x40,
		ASSET_TYPE_ANIMSELECTORTABLESET = 0x41,
		ASSET_TYPE_ANIMMAPPINGTABLE = 0x42,
		ASSET_TYPE_ANIMSTATEMACHINE = 0x43,
		ASSET_TYPE_BEHAVIORTREE = 0x44,
		ASSET_TYPE_BEHAVIORSTATEMACHINE = 0x45,
		ASSET_TYPE_TTF = 0x46,
		ASSET_TYPE_SANIM = 0x47,
		ASSET_TYPE_LIGHT_DESCRIPTION = 0x48,
		ASSET_TYPE_SHELLSHOCK = 0x49,
		ASSET_TYPE_CINEMATIC_CAMERA = 0x4A,
		ASSET_TYPE_XCAM = 0x4B,
		ASSET_TYPE_BG_CACHE = 0x4C,
		ASSET_TYPE_TEXTURE_COMBO = 0x4D,
		ASSET_TYPE_FLAMETABLE = 0x4E,
		ASSET_TYPE_BITFIELD = 0x4F,
		ASSET_TYPE_ATTACHMENT_COSMETIC_VARIANT = 0x50,
		ASSET_TYPE_MAPTABLE = 0x51,
		ASSET_TYPE_MAPTABLE_LOADING_IMAGES = 0x52,
		ASSET_TYPE_MAPTABLEENTRY_LEVEL_ASSETS = 0x53,
		ASSET_TYPE_MEDAL = 0x54,
		ASSET_TYPE_MEDALTABLE = 0x55,
		ASSET_TYPE_OBJECTIVE = 0x56,
		ASSET_TYPE_OBJECTIVE_LIST = 0x57,
		ASSET_TYPE_NAVMESH = 0x58,
		ASSET_TYPE_NAVVOLUME = 0x59,
		ASSET_TYPE_BINARYHTML = 0x5A,
		ASSET_TYPE_LASER = 0x5B,
		ASSET_TYPE_BEAM = 0x5C,
		ASSET_TYPE_STREAMER_HINT = 0x5D,
		ASSET_TYPE_FLOWGRAPH = 0x5E,
		ASSET_TYPE_POSTFXBUNDLE = 0x5F,
		ASSET_TYPE_LUAFILE = 0x60,
		ASSET_TYPE_LUAFILE_DBG = 0x61,
		ASSET_TYPE_RENDEROVERRIDEBUNDLE = 0x62,
		ASSET_TYPE_STATIC_LEVEL_FX_LIST = 0x63,
		ASSET_TYPE_TRIGGER_LIST = 0x64,
		ASSET_TYPE_PLAYER_ROLE_TEMPLATE = 0x65,
		ASSET_TYPE_PLAYER_ROLE_CATEGORY_TABLE = 0x66,
		ASSET_TYPE_PLAYER_ROLE_CATEGORY = 0x67,
		ASSET_TYPE_CHARACTER_BODY_TYPE = 0x68,
		ASSET_TYPE_GAMETYPETABLE = 0x69,
		ASSET_TYPE_UNLOCKABLE_ITEM_TABLE = 0x6A,
		ASSET_TYPE_ENTITY_LIST = 0x6B,
		ASSET_TYPE_PLAYLISTS = 0x6C,
		ASSET_TYPE_PLAYLIST_GLOBAL_SETTINGS = 0x6D,
		ASSET_TYPE_PLAYLIST_EVENT = 0x6E,
		ASSET_TYPE_PLAYLIST_EVENT_SCHEDULE = 0x6F,
		ASSET_TYPE_MOTION_MATCHING_INPUT = 0x70,
		ASSET_TYPE_BLACKBOARD = 0x71,
		ASSET_TYPE_TACTICALQUERY = 0x72,
		ASSET_TYPE_PMOVE = 0x73,
		ASSET_TYPE_HIERARCHICAL_TASK_NETWORK = 0x74,
		ASSET_TYPE_RAGDOLL = 0x75,
		ASSET_TYPE_STORAGEFILE = 0x76,
		ASSET_TYPE_STORAGEFILELIST = 0x77,
		ASSET_TYPE_CHARMIXER = 0x78,
		ASSET_TYPE_COUNT = 0x79,
		ASSET_TYPE_ASSETLIST = 0x79,
		ASSET_TYPE_REPORT = 0x7A,
		ASSET_TYPE_FULL_COUNT = 0x7B,
	};

	enum SearchState
	{
		SEARCH_STATE_IDLE = 0x0,
		SEARCH_STATE_SEARCH = 0x1,
		SEARCH_STATE_SEARCHING = 0x2,
		SEARCH_STATE_COMPLETE = 0x3,
	};

	enum hitLocation_t
	{
		HITLOC_NONE = 0x0,
		HITLOC_HELMET = 0x1,
		HITLOC_HEAD = 0x2,
		HITLOC_NECK = 0x3,
		HITLOC_TORSO_UPR = 0x4,
		HITLOC_TORSO_MID = 0x5,
		HITLOC_TORSO_LWR = 0x6,
		HITLOC_R_ARM_UPR = 0x7,
		HITLOC_L_ARM_UPR = 0x8,
		HITLOC_R_ARM_LWR = 0x9,
		HITLOC_L_ARM_LWR = 0xA,
		HITLOC_R_HAND = 0xB,
		HITLOC_L_HAND = 0xC,
		HITLOC_R_LEG_UPR = 0xD,
		HITLOC_L_LEG_UPR = 0xE,
		HITLOC_R_LEG_LWR = 0xF,
		HITLOC_L_LEG_LWR = 0x10,
		HITLOC_R_FOOT = 0x11,
		HITLOC_L_FOOT = 0x12,
		HITLOC_GUN = 0x13,
		HITLOC_SHIELD = 0x14,
		HITLOC_NUM = 0x15,
	};
	
	enum NetChanMsgType
	{
		NETCHAN_INVALID_CHANNEL = 0xFFFFFFFF,
		NETCHAN_SNAPSHOT = 0x0,
		NETCHAN_CLIENTMSG = 0x1,
		NETCHAN_VOICE = 0x2,
		NETCHAN_LOBBY_VOICE = 0x3,
		NETCHAN_LOBBYPRIVATE_STATE = 0x4,
		NETCHAN_LOBBYPRIVATE_HEARTBEAT = 0x5,
		NETCHAN_LOBBYPRIVATE_RELIABLE = 0x6,
		NETCHAN_LOBBYPRIVATE_UNRELIABLE = 0x7,
		NETCHAN_LOBBYPRIVATE_MIGRATE = 0x8,
		NETCHAN_LOBBYGAME_STATE = 0x9,
		NETCHAN_LOBBYGAME_HEARTBEAT = 0xA,
		NETCHAN_LOBBYGAME_RELIABLE = 0xB,
		NETCHAN_LOBBYGAME_UNRELIABLE = 0xC,
		NETCHAN_LOBBYGAME_MIGRATE = 0xD,
		NETCHAN_LOBBYTRANSITION_STATE = 0xE,
		NETCHAN_LOBBYTRANSITION_HEARTBEAT = 0xF,
		NETCHAN_LOBBYTRANSITION_RELIABLE = 0x10,
		NETCHAN_LOBBYTRANSITION_UNRELIABLE = 0x11,
		NETCHAN_LOBBYTRANSITION_MIGRATE = 0x12,
		NETCHAN_LOBBY_JOIN = 0x13,
		NETCHAN_PTP = 0x14,
		NETCHAN_CLIENT_CONTENT = 0x15,
		NETCHAN_CLIENT_CMD = 0x16,
		NETCHAN_CONNECTIONLESS_CMD = 0x17,
		NETCHAN_TEST = 0x18,
		NETCHAN_MAX_CHANNELS = 0x19,
	}; 

	enum JoinType
	{
		JOIN_TYPE_NORMAL = 0x0,
		JOIN_TYPE_PLAYLIST = 0x1,
		JOIN_TYPE_FRIEND = 0x2,
		JOIN_TYPE_INVITE = 0x3,
		JOIN_TYPE_PARTY = 0x4,
		JOIN_TYPE_GROUPS = 0x5,
		JOIN_TYPE_COUNT = 0x6,
	};
	
	enum team_t
	{
		TEAM_FREE = 0x0,
		TEAM_BAD = 0x0,
		TEAM_DEAD = 0x0,
		TEAM_ALLIES = 0x1,
		TEAM_AXIS = 0x2,
		TEAM_THREE = 0x3,
		TEAM_FOUR = 0x4,
		TEAM_FIVE = 0x5,
		TEAM_SIX = 0x6,
		TEAM_SEVEN = 0x7,
		TEAM_EIGHT = 0x8,
		TEAM_NINE = 0x9,
		TEAM_TEN = 0xA,
		TEAM_ELEVEN = 0xB,
		TEAM_TWELVE = 0xC,
		TEAM_THIRTEEN = 0xD,
		TEAM_FOURTEEN = 0xE,
		TEAM_FIFTEEN = 0xF,
		TEAM_SIXTEEN = 0x10,
		TEAM_SEVENTEEN = 0x11,
		TEAM_EIGHTEEN = 0x12,
		TEAM_NEUTRAL = 0x13,
		TEAM_SPECTATOR = 0x14,
		TEAM_NUM_PLAYING_TEAMS = 0x14,
		TEAM_NUM_TEAMS = 0x15,
		TEAM_LOCALPLAYERS = 0x16,
		TEAM_FIRST_PLAYING_TEAM = 0x1,
		TEAM_LAST_PLAYING_TEAM = 0x13,
	};

	enum PartyPrivacy
	{
		PARTY_PRIVACY_OPEN = 0x0,
		PARTY_PRIVACY_FRIENDS_ONLY = 0x1,
		PARTY_PRIVACY_INVITE_ONLY = 0x2,
		PARTY_PRIVACY_CLOSED = 0x3,
		PARTY_PRIVACY_COUNT = 0x4,
	};

	enum SessionStatus
	{
		SESSION_STATUS_INVALID = 0xFFFFFFFF,
		SESSION_STATUS_IDLE = 0x0,
		SESSION_STATUS_JOIN = 0x1,
		SESSION_STATUS_MERGE = 0x2,
		SESSION_STATUS_MIGRATE = 0x3,
		SESSION_STATUS_PAUSED = 0x4,
		SESSION_STATUS_COUNT = 0x5,
	};
	
	enum errorCode
	{
		ERROR_NONE = 0x0,
		ERROR_FATAL = 0x1,
		ERROR_DROP = 0x2,
		ERROR_FROM_STARTUP = 0x4,
		ERROR_SERVERDISCONNECT = 0x8,
		ERROR_DISCONNECT = 0x10,
		ERROR_SCRIPT = 0x20,
		ERROR_SCRIPT_DROP = 0x40,
		ERROR_LOCALIZATION = 0x80,
		ERROR_UI = 0x100,
		ERROR_LUA = 0x200,
		ERROR_SOFTRESTART = 0x400,
		ERROR_SOFTRESTART_KEEPDW = 0x800,
		ERROR_SOFTRESTART_SILENT = 0x1000,
	};

	enum TraceBrushType
	{
		TRACE_BRUSHTYPE_NONE = 0x0,
		TRACE_BRUSHTYPE_BRUSH = 0x1,
	};
	
	enum TraceHitType
	{
		TRACE_HITTYPE_NONE = 0x0,
		TRACE_HITTYPE_ENTITY = 0x1,
		TRACE_HITTYPE_DYNENT_MODEL = 0x2,
		TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
		TRACE_HITTYPE_GLASS = 0x4,
	};
	
	enum dvarType_t
	{
		DVAR_TYPE_INVALID = 0x0,
		DVAR_TYPE_BOOL = 0x1,
		DVAR_TYPE_FLOAT = 0x2,
		DVAR_TYPE_FLOAT_2 = 0x3,
		DVAR_TYPE_FLOAT_3 = 0x4,
		DVAR_TYPE_FLOAT_4 = 0x5,
		DVAR_TYPE_INT = 0x6,
		DVAR_TYPE_ENUM = 0x7,
		DVAR_TYPE_STRING = 0x8,
		DVAR_TYPE_COLOR = 0x9,
		DVAR_TYPE_INT64 = 0xA,
		DVAR_TYPE_UINT64 = 0xB,
		DVAR_TYPE_LINEAR_COLOR_RGB = 0xC,
		DVAR_TYPE_COLOR_XYZ = 0xD,
		DVAR_TYPE_COLOR_LAB = 0xE,
		DVAR_TYPE_SESSIONMODE_BASE_DVAR = 0xF,
		DVAR_TYPE_COUNT = 0x10,
	};

	enum LobbyModule
	{
		LOBBY_MODULE_INVALID = 0xFFFFFFFF,
		LOBBY_MODULE_HOST = 0x0,
		LOBBY_MODULE_CLIENT = 0x1,
		LOBBY_MODULE_PEER_TO_PEER = 0x3,
		LOBBY_MODULE_COUNT = 0x4,
	};

	enum LobbyType
	{
		LOBBY_TYPE_INVALID = 0xFFFFFFFF,
		LOBBY_TYPE_PRIVATE = 0x0,
		LOBBY_TYPE_GAME = 0x1,
		LOBBY_TYPE_TRANSITION = 0x2,
		LOBBY_TYPE_COUNT = 0x3,
	};

	enum eGameModes
	{
		MODE_GAME_MATCHMAKING_PLAYLIST = 0x0,
		MODE_GAME_MATCHMAKING_MANUAL = 0x1,
		MODE_GAME_DEFAULT = 0x2,
		MODE_GAME_LEAGUE = 0x3,
		MODE_GAME_FREERUN = 0x4,
		MODE_GAME_THEATER = 0x5,
		MODE_GAME_COUNT = 0x6,
		MODE_GAME_INVALID = 0x6,
	}; 
	
	enum MsgType
	{
		MESSAGE_TYPE_NONE = -1,
		MESSAGE_TYPE_INFO_REQUEST = 0,
		MESSAGE_TYPE_INFO_RESPONSE = 1,
		MESSAGE_TYPE_LOBBY_STATE_PRIVATE = 2,
		MESSAGE_TYPE_LOBBY_STATE_GAME = 3,
		MESSAGE_TYPE_LOBBY_STATE_TRANSITION = 4,
		MESSAGE_TYPE_LOBBY_HOST_HEARTBEAT = 7,
		MESSAGE_TYPE_LOBBY_HOST_DISCONNECT = 8,
		MESSAGE_TYPE_LOBBY_HOST_DISCONNECT_CLIENT = 9,
		MESSAGE_TYPE_LOBBY_HOST_LEAVE_WITH_PARTY = 10,
		MESSAGE_TYPE_LOBBY_CLIENT_HEARTBEAT = 11,
		MESSAGE_TYPE_LOBBY_CLIENT_DISCONNECT = 12,
		MESSAGE_TYPE_LOBBY_CLIENT_RELIABLE_DATA = 13,
		MESSAGE_TYPE_LOBBY_CLIENT_CONTENT = 14,
		MESSAGE_TYPE_LOBBY_MODIFIED_STATS = 15,
		MESSAGE_TYPE_JOIN_LOBBY = 16,
		MESSAGE_TYPE_JOIN_RESPONSE = 17,
		MESSAGE_TYPE_JOIN_AGREEMENT_REQUEST = 18,
		MESSAGE_TYPE_JOIN_AGREEMENT_RESPONSE = 19,
		MESSAGE_TYPE_JOIN_COMPLETE = 20,
		MESSAGE_TYPE_JOIN_MEMBER_INFO = 21,
		MESSAGE_TYPE_SERVERLIST_INFO = 22,
		MESSAGE_TYPE_PEER_TO_PEER_CONNECTIVITY_TEST = 23,
		MESSAGE_TYPE_PEER_TO_PEER_INFO = 24,
		MESSAGE_TYPE_LOBBY_MIGRATE_TEST = 25,
		MESSAGE_TYPE_LOBBY_MIGRATE_ANNOUNCE_HOST = 26,
		MESSAGE_TYPE_LOBBY_MIGRATE_START = 27,
		MESSAGE_TYPE_INGAME_MIGRATE_TO = 28,
		MESSAGE_TYPE_INGAME_MIGRATE_NEW_HOST = 29,
		MESSAGE_TYPE_VOICE_PACKET = 30,
		MESSAGE_TYPE_DEMO_STATE = 32,
		MESSAGE_TYPE_COUNT = 33,
	};

	enum PackageType
	{
		PACKAGE_TYPE_NONE = 0x0,
		PACKAGE_TYPE_WRITE = 0x1,
		PACKAGE_TYPE_READ = 0x2,
	};

	enum IMType
	{
		JOIN_REQUEST = 0x0,
		JOIN_REPLY = 0x1,
		INVITE = 0x2,
		UPDATE_INVITE_INFO = 0x3,
		NUM_INSTANT_MESSAGE_TYPES = 0x4,
	};

	enum JoinRejectionReason
	{
		REASON_NONE = 0x0,
		REASON_PARTY_PRIVACY_CLOSED = 0x1,
		REASON_PARTY_PRIVACY_INVITE_ONLY = 0x2,
		REASON_SYSTEMLINK_LOBBY = 0x3,
		REASON_SPLITSCREEN_LOBBY = 0x4,
	};

	enum LobbyMode
	{
		LOBBY_MODE_INVALID = 0xFFFFFFFF,
		LOBBY_MODE_PUBLIC = 0x0,
		LOBBY_MODE_CUSTOM = 0x1,
		LOBBY_MODE_THEATER = 0x2,
		LOBBY_MODE_ARENA = 0x3,
		LOBBY_MODE_FREERUN = 0x4,
		LOBBY_MODE_COUNT = 0x5,
	};

	enum SessionActive
	{
		SESSION_INACTIVE = 0x0,
		SESSION_KEEP_ALIVE = 0x1,
		SESSION_ACTIVE = 0x2,
	};
	
	struct $7034703ED3857507327AE195CCA24A71
	{
		int stringCount;
		const char **strings;
	};
	
	struct $5AC0CC375C7A91894540A0C75867866C
	{
		int min;
		int max;
	};

	struct $CE635080F256838B60294DD7E0543359
	{
		int64_t min;
		int64_t max;
	};

	struct $28DE879AA85D5F4AA5DF9392CB197229
	{
		uint64_t min;
		uint64_t max;
	};

	struct $D2B45DCE3C73414E1DE8E10A2D52B392
	{
		float min;
		float max;
	};

	struct $97F25DA469865168FD08682F42882F2A
	{
		float min;
		float max;
	};
	
	union DvarLimits
	{
		$7034703ED3857507327AE195CCA24A71 enumeration;
		$5AC0CC375C7A91894540A0C75867866C integer;
		$CE635080F256838B60294DD7E0543359 integer64;
		$28DE879AA85D5F4AA5DF9392CB197229 unsignedInt64;
		$D2B45DCE3C73414E1DE8E10A2D52B392 value;
		$97F25DA469865168FD08682F42882F2A vector;
	};
	
	union DvarValue
	{
		bool enabled;
		int integer;
		unsigned int unsignedInt;
		int64_t integer64;
		uint64_t unsignedInt64;
		float value;
		Vec3 vector;
		const char *string;
		float color[4];
	}; 
	
	struct dvar_t
	{
		dvarStrHash_t name;
		const char *debugName;
		const char *description;
		unsigned int flags;
		dvarType_t type;
		bool modified;
		DvarValue current;
		DvarValue latched;
		DvarValue reset;
		DvarLimits domain;
		dvar_t *hashNext;
	};

	enum PacketModeList
	{
		PACKETDATA_FIRST = 0x0,
		PACKETDATA_UNDEFINED = 0x0,
		PACKETDATA_HEADER = 0x1,
		PACKETDATA_OVERHEAD = 0x2,
		PACKETDATA_DATA = 0x3,
		PACKETDATA_RELIABLEDATA = 0x4,
		PACKETDATA_ZEROFLOAT = 0x5,
		PACKETDATA_SMALLFLOAT = 0x6,
		PACKETDATA_LARGEFLOAT = 0x7,
		PACKETDATA_ZEROINT = 0x8,
		PACKETDATA_SMALLANGLE = 0x9,
		PACKETDATA_ZEROANGLE = 0xA,
		PACKETDATA_TIMEDELTA = 0xB,
		PACKETDATA_TIME = 0xC,
		PACKETDATA_24BITFLAGINDEX = 0xD,
		PACKETDATA_GROUNDENTITY = 0xE,
		PACKETDATA_ENTITYNUM = 0xF,
		PACKETDATA_LASTFIELDCHANGED = 0x10,
		PACKETDATA_NOTNETWORKDATA = 0x11,
		PACKETDATA_ORIGINDELTA = 0x12,
		PACKETDATA_ORIGIN = 0x13,
		NUM_PACKETDATA_MODES = 0x14,
	};

	enum netsrc_t
	{
		NS_NULL = 0xFFFFFFFF,
		NS_CLIENT1 = 0x0,
		NS_CLIENT2 = 0x1,
		NS_CLIENT3 = 0x2,
		NS_CLIENT4 = 0x3,
		NS_SERVER = 0x4,
		NS_MAXCLIENTS = 0x4,
		NS_PACKET = 0x5,
	};

	enum PenetrateType
	{
		PENETRATE_TYPE_NONE = 0x0,
		PENETRATE_TYPE_SMALL = 0x1,
		PENETRATE_TYPE_MEDIUM = 0x2,
		PENETRATE_TYPE_LARGE = 0x3,
		PENETRATE_TYPE_COUNT = 0x4,
	};

	enum weapClass_t : __int32
	{
		WEAPCLASS_RIFLE = 0x0,
		WEAPCLASS_MG = 0x1,
		WEAPCLASS_SMG = 0x2,
		WEAPCLASS_SPREAD = 0x3,
		WEAPCLASS_PISTOL = 0x4,
		WEAPCLASS_GRENADE = 0x5,
		WEAPCLASS_ROCKETLAUNCHER = 0x6,
		WEAPCLASS_TURRET = 0x7,
		WEAPCLASS_NON_PLAYER = 0x8,
		WEAPCLASS_GAS = 0x9,
		WEAPCLASS_ITEM = 0xA,
		WEAPCLASS_MELEE = 0xB,
		WEAPCLASS_KILLSTREAK_ALT_STORED_WEAPON = 0xC,
		WEAPCLASS_PISTOL_SPREAD = 0xD,
		WEAPCLASS_BALL = 0xE,
		WEAPCLASS_NUM = 0xF,
	};

	enum weapType_t
	{
		WEAPTYPE_BULLET = 0x0,
		WEAPTYPE_GRENADE = 0x1,
		WEAPTYPE_PROJECTILE = 0x2,
		WEAPTYPE_BINOCULARS = 0x3,
		WEAPTYPE_GAS = 0x4,
		WEAPTYPE_BOMB = 0x5,
		WEAPTYPE_MINE = 0x6,
		WEAPTYPE_MELEE = 0x7,
		WEAPTYPE_RIOTSHIELD = 0x8,
		WEAPTYPE_NUM = 0x9,
	};

	enum netadrtype_t
	{
		NA_BOT = 0x0,
		NA_BAD = 0x1,
		NA_LOOPBACK = 0x2,
		NA_RAWIP = 0x3,
		NA_IP = 0x4,
	};

	enum LobbyDisconnectClient
	{
		LOBBY_DISCONNECT_CLIENT_INVALID = 0xFFFFFFFF,
		LOBBY_DISCONNECT_CLIENT_DROP = 0x0,
		LOBBY_DISCONNECT_CLIENT_KICK = 0x1,
		LOBBY_DISCONNECT_CLIENT_BADDLC = 0x2,
		LOBBY_DISCONNECT_CLIENT_KICK_PARTY = 0x3,
		LOBBY_DISCONNECT_CLIENT_HOSTRELOAD = 0x4,
		LOBBY_DISCONNECT_CLIENT_NOPARTYCHAT = 0x5,
	};

	enum NetchanMsgMode
	{
		NETCHAN_RELIABLE = 0x0,
		NETCHAN_UNRELIABLE = 0x1,
	};

	enum LobbyChannel
	{
		LOBBY_CHANNEL_HEARTBEAT = 0x0,
		LOBBY_CHANNEL_STATE = 0x1,
		LOBBY_CHANNEL_UNRELIABLE = 0x2,
		LOBBY_CHANNEL_RELIABLE = 0x3,
		LOBBY_CHANNEL_MIGRATE = 0x4,
		LOBBY_CHANNEL_PEER_TO_PEER = 0x5,
		LOBBY_CHANNEL_COUNT = 0x6,
	};

	enum LobbyNetworkMode
	{
		LOBBY_NETWORKMODE_INVALID = 0xFFFFFFFF,
		LOBBY_NETWORKMODE_UNKNOWN = 0x0,
		LOBBY_NETWORKMODE_LAN = 0x1,
		LOBBY_NETWORKMODE_LIVE = 0x2,
		LOBBY_NETWORKMODE_COUNT = 0x3,
		LOBBY_NETWORKMODE_LOCAL = 0x1,
	};

	enum LobbyMainMode
	{
		LOBBY_MAINMODE_INVALID = 0xFFFFFFFF,
		LOBBY_MAINMODE_CP = 0x0,
		LOBBY_MAINMODE_MP = 0x1,
		LOBBY_MAINMODE_ZM = 0x2,
		LOBBY_MAINMODE_COUNT = 0x3,
	};

	enum eModes
	{
		MODE_ZOMBIES = 0x0,
		MODE_MULTIPLAYER = 0x1,
		MODE_CAMPAIGN = 0x2,
		MODE_COUNT = 0x3,
		MODE_INVALID = 0x3,
		MODE_FIRST = 0x0,
	};

	struct msg_t
	{
		int overflowed;
		int readOnly;
		char *data;
		char *splitData;
		int maxsize;
		int cursize;
		int splitSize;
		int readcount;
		int bit;
		int lastEntityRef;
		int flush;
		netsrc_t targetLocalNetID;
		MsgType type;
		char encodeFlags;
		PackageType packageType;
	};
	
	struct netadr_t
	{
		union
		{
			std::uint8_t ip[4];
			std::uint32_t inaddr;
		};
		
		std::uint16_t port;
		netadrtype_t type;
		netsrc_t localNetID;
	};

	struct va_info_t
	{
		char va_string[4][1024];
		int index;
	};
	
	struct TraceCheckCount
	{
		unsigned __int16 global;
		unsigned __int16 *partitions;
		unsigned __int16 *brushes;
	};
	
	struct TraceThreadInfo
	{
		TraceCheckCount checkcount;
		void *box_brush;
		void *box_model;
		void **geoms;
	};
	
	struct CmdArgs
	{
		int nesting;
		LocalClientNum_t localClientNum[8];
		ControllerIndex_t controllerIndex[8];
		int argshift[8];
		int argc[8];
		const char **argv[8];
		char textPool[8192];
		const char *argvPool[512];
		int usedTextPool[8];
		int totalUsedArgvPool;
		int totalUsedTextPool;
	};
	
	struct TLSData
	{
		va_info_t *vaInfo;
		jmp_buf *errorJmpBuf;
		TraceThreadInfo *traceInfo;
		CmdArgs *cmdArgs;
		void *errorData;
	};

#pragma pack(push, 1)
	struct XNADDR
	{
		char pad[0x1E];
		union
		{
			std::uint8_t ip[4];
			std::uint32_t inaddr;
		};

		std::uint16_t port;
		char pad2[0x1];
	};
#pragma pack(pop)

	struct bdSecurityID
	{
		std::uint64_t id;
	};

	struct bdSecurityKey
	{
		char ab[16];
	};
	
	struct XSESSION_INFO
	{
		bdSecurityID sessionID;
		XNADDR hostAddress;
		bdSecurityKey keyExchangeKey;
	};
	
	struct JoinSessionMessage
	{
		IMType mType;
		char pad[0x60];
	};

	struct SessionInfo
	{
		bool inSession;
		netadr_t netAdr;
		time_t lastMessageSentToPeer;
	};

	struct FixedClientInfo
	{
		std::uint64_t xuid;
		char gamertag[32];
	};

	struct MutableClientInfo
	{
		std::uint64_t lobbyID;
		char clantag[5];
	};
	
	struct ActiveClient
	{
		MutableClientInfo mutableClientInfo;
		char pad[0x400];
		FixedClientInfo fixedClientInfo;
		char pad2[0xA8];
		SessionInfo sessionInfo[3];
	};

	struct MatchInfo
	{
		uint32_t score;
	};
	
	struct VoiceChatInfo
	{
		int32_t lastCaughtTalking;
		int32_t connectivityBits;
		bool isGameRestricted;
	};
	
	struct SessionClient
	{
		std::uint64_t xuid;
		ActiveClient *activeClient;
		MatchInfo matchInfo;
		VoiceChatInfo voiceInfo;
		uint32_t joinOrder;
		uint64_t joinNonce;
	};

	struct LobbyParams
	{
		LobbyNetworkMode networkMode;
		LobbyMainMode mainMode;
	};

	struct SerializedAdr
	{
		bool valid;
		game::XNADDR xnaddr;
	};

	struct InfoResponseLobby
	{
		bool isValid;
		std::uint64_t hostXuid;
		char hostName[32];
		bdSecurityID secId;
		bdSecurityKey secKey;
		SerializedAdr serializedAdr;
		LobbyParams lobbyParams;
		char pad[0x28];
	};

	struct Msg_InfoResponse
	{
		std::uint32_t nonce;
		int uiScreen;
		std::uint8_t natType;
		InfoResponseLobby lobby[2];
	};

	struct HostInfo
	{
		std::uint64_t xuid;
		char name[32];
		netadr_t netAdr;
		SerializedAdr serializedAdr;
		bdSecurityID secId;
		bdSecurityKey secKey;
		uint32_t serverLocation;
	};

	struct SessionHost
	{
		HostInfo info;
		int maxClients;
	};

	struct SessionLeader
	{
		std::uint64_t xuid;
	};
	
	struct LobbySession
	{
		LobbyModule module;
		LobbyType type;
		LobbyMode mode;
		char pad[0x34];
		SessionActive active;
		char pad2[0x18];
		SessionHost host;
		SessionLeader leader;
		uint32_t joinCount;
		SessionClient clients[18];
		char pad3[0x2600];
	};
	
	struct playerState_s
	{
		ClientNum_t clientNum;
		int commandTime;
		char pad[0x28];
		Vec3 origin;
		char pad2[0x2C0];
		float fWeaponPosFrac;
		char pad3[0xB274];
	};

	struct trajectory_t
	{
		char trType;
		int trTime;
		int trDuration;
		Vec3 trBase;
		Vec3 trDelta;
	};
	
	struct LerpEntityState
	{
		int eFlags;
		int eFlags2;
		trajectory_t pos;
		trajectory_t apos;
	};
	
	struct entityState_t
	{
		int number;
		LerpEntityState lerp;
	};

	struct cpose_t
	{
		char pad[0x40];
		Vec3 origin;
		Vec3 angles;
		char pad2[0x3A0];
	};

	struct centity_t
	{
		cpose_t pose;
		entityState_t nextState;
		char pad2[0x4A8];
		std::uint8_t alive;
		char pad3[0x8];

		bool is_alive() const {
			return alive & 2;
		}
	};

	struct clientInfo_t
	{
		int infoValid;
		int nextValid;
		ClientNum_t clientNum;
		char name[32];
		team_t team;
		team_t oldteam; 
		char pad[0xC0];
		int health;
		char pad2[0xDD8];
	};

	struct cg_t
	{
		int clientNum;
		char pad[0x11A8AC];
		playerState_s predictedPlayerState;
		int lastPlayerStateOverride;
		centity_t predictedPlayerEntity;
		char pad2[0x1B21B8];
		Vec3 refdefViewAngles;
		char pad3[0x7E8];
		Vec3 baseGunAngles;
		char pad4[0xE708];
		bool inKillcam;
		char pad5[0x254];
		clientInfo_t clients[18];
		char pad6[0x2E5C0];
		float aimSpreadScale;
		char pad7[0x1BC7C];
	};

	struct usercmd_s
	{
		int serverTime;
		int button_bits[3];
		int angles[3];
		char pad[0x4];
		Weapon weapon;
		Weapon offHandWeapon;
		Weapon lastWeaponAltModeSwitch;
		char forwardmove;
		char rightmove;
		char upmove;
		char pitchmove;
		char yawmove;
		char pad2[0xE];

		Vec3 get_angles() const
		{
			Vec3 result{};

			for (size_t i = 0; i < std::size(angles); ++i)
			{
				result[i] = SHORT2ANGLE(angles[i]);
			}

			return result;
		}

		void set_angles(const Vec3& value)
		{
			for (size_t i = 0; i < std::size(angles); ++i)
			{
				angles[i] = ANGLE2SHORT(value[i]);
			}
		}
	};

	struct clientActive_t
	{
		char pad[0xB674];
		int serverId;
		char pad2[0x160];
		Vec3 cgameKickAngles;
		char pad3[0xE4];
		Vec3 viewangles;
		char pad4[0x10001C];
		usercmd_s cmds[128];
		int cmdNumber;
		char pad5[0x8993C];

		usercmd_s* user_cmd(const int i)
		{
			return &cmds[i & 0x7F];
		}
	};

	struct Msg_ClientReliableData
	{
		uint32_t dataMask;
		int lobbyType;
		uint64_t xuidNewLeader;
		uint64_t disconnectClientXuid;
		int disconnectClient;
	};

	struct trace_t
	{
		Vec3 normal;
		char _0xC[0x4];
		float fraction;
		int sflags;
		char _0x18[0xC];
		TraceHitType hitType;
		std::uint16_t hitId;
		char _0x2A[0xC];
		uint16_t partGroup;
		bool allSolid;
		bool startSolid;
		char _0x3A[0x16];
		int hitPartition;
	};

	struct BulletTraceResults
	{
		trace_t trace;
		Vec3 hitPos;
		bool ignoreHitEnt;
		char pad[0x3];
		int depthSurfaceType;
		char pad2[0x8];
	};

	struct BulletFireParams
	{
		int weaponEntIndex;
		int ignoreEntIndex;
		float damageMultiplier;
		int methodOfDeath;
		Vec3 origStart;
		Vec3 start;
		Vec3 end;
		Vec3 dir;
		Vec3 origDir;
	};

	struct BulletHitInfo
	{
		BulletFireParams bp;
		char pad[0x4];
		BulletTraceResults br;
		BulletFireParams revBp;
		char pad2[0x4];
		BulletTraceResults revBr;
		bool traceHit;
		bool revTraceHit;
		bool allSolid;
		bool voidFilled;
		int sflags;
		int sflagsRv;
		float damage;
		char pad3[0x4];
		float range;
		char pad4[0x8];
	}; 
	
	struct BulletPenetrationInfo
	{
		BulletHitInfo hits[5];
		int count;
		char pad[0xC];
	};
	
#pragma pack(push, 1)
	struct WeaponDef
	{
		char pad[0x6C];
		weapType_t weapType;
		weapClass_t weapClass;
		char pad2[0x422];
		float fAdsSpread;
		char pad3[0xC33];
		bool bBulletImpactExplode;
	};
#pragma pack(pop)

	struct InfoProbe
	{
		bool active;
		unsigned int nonce;
	};

	struct MsgClientMigrateInfo
	{
		uint16_t totalLatencyMS;
		uint32_t recievedPacketCount;
		float m;
		uint32_t uploadBandwidth;
	};

	struct MsgMutableClientInfo
	{
		MutableClientInfo mutableClientInfo;
		char pad[0x400];
	};

	struct Msg_LobbyClientHeartbeat
	{
		int lobbyType;
		MsgMutableClientInfo mutableClientInfoMsg;
		MsgClientMigrateInfo migrateInfo;
	};

	struct Msg_ClientContent
	{
		uint32_t dataMask;
		int lobbyType;
		char clientContentData[162816];
		uint16_t compressedBufferSize;
		uint64_t clientXUID;
		eModes sessionMode;
		eGameModes gameMode;
	};

	struct JoinPartyMember
	{
		uint64_t xuid;
		uint64_t lobbyID;
		float skillRating;
		float skillVariance;
		uint32_t probationTimeRemaining[2];
	};

	struct Msg_JoinParty
	{
		int targetLobby;
		int sourceLobby;
		JoinType joinType;
		uint64_t probedXuid;
		int32_t memberCount;
		JoinPartyMember members[18];
		int32_t splitscreenClients;
		int32_t playlistID;
		int32_t playlistVersion[3];
		int32_t ffotdVersion;
		LobbyNetworkMode networkMode;
		uint32_t netFieldChecksum;
		int32_t protocolVersion;
		int32_t changelist;
		int32_t pingBand;
		uint32_t dlcBits;
		uint64_t joinNonce;
		bool chunkStatus[3];
	};

	struct MsgFixedClientInfo
	{
		FixedClientInfo fixedClientInfo;
		char pad2[0xA8];
	}; 
	
	struct Msg_JoinMemberInfo
	{
		MsgMutableClientInfo mutableClientInfoMsg;
		MsgFixedClientInfo fixedClientInfoMsg;
		uint64_t reservationKey;
		int targetLobby;
		SerializedAdr serializedAdr;
	};

	struct MsgHostMigrateInfo
	{
		uint8_t indexBits;
		int32_t lasthostTimeMS;
		uint64_t migrateNominees[18];
	}; 
	
	struct PlatformSessionData
	{
		char sessionId[64];
		char sessionInfo[64];
	};

	struct MsgPlatformSessionData
	{
		PlatformSessionData platformSessionData;
	};
	
	struct MsgClientLobbyInfo
	{
		uint64_t xuid;
		uint8_t clientNum;
		int8_t clientType;
		char gamertag[32];
		uint64_t lobbyID;
		int32_t qport;
		byte latencyBand;
		MsgMutableClientInfo mutableClientInfoMsg;
		int32_t score;
		int32_t connectivityBits;
		SerializedAdr serializedAdr;
		netsrc_t localNetID;
		uint32_t joinOrder;
		uint32_t dlcBits;
	}; 
	
	struct Msg_LobbyState
	{
		int stateNum;
		LobbyNetworkMode lobbyNetworkMode;
		LobbyMainMode lobbyMainMode;
		PartyPrivacy partyPrivacy;
		LobbyType lobbyType;
		LobbyMode lobbyMode;
		SessionStatus status;
		int uiScreen;
		int uiLeaderActivity;
		char key[32];
		uint64_t clientLeader;
		uint64_t platformSessionID;
		int maxClients;
		bool isAdvertised;
		int clientCount;
		MsgClientLobbyInfo clientList[18];
		MsgHostMigrateInfo migrateInfo;
		MsgPlatformSessionData platformData;
	};

	struct bdMatchMakingInfo
	{
		bdSecurityID sessionID;
		std::uint8_t hostAddr[255];
		uint32_t hostAddrSize;
		uint32_t gameType;
		uint32_t maxPlayers;
		uint32_t numPlayers;
	};
	
	struct MatchMakingInfo
	{
		char pad[0x20];
		bdMatchMakingInfo info;
		uint64_t sessionID;
		char keyExchangeKey[17];
		int serverType;
		uint64_t xuid;
		uint32_t serverLocation;
		uint32_t latencyBand;
		int showInMatchmaking;
		int netcodeVersion;
		int mapPacks;
		int playlistVersion;
		int playlistNumber;
		int isEmpty;
		int teamMax;
		float skill;
		int geo1;
		int geo2;
		int geo3;
		int geo4;
		int dirty;
		int active;
		int timeSinceLastUpdate;
		int recreateSession;
		uint32_t timeSinceUpdate;
		char pad2[0x14];
	};

	struct GfxDrawSurfFields
	{
		unsigned __int64 useSiegeTextures : 1;
		unsigned __int64 objectId : 18;
		unsigned __int64 lateDepthSort : 5;
		unsigned __int64 materialSortedIndex : 16;
		unsigned __int64 techniqueIndexPlusOne : 11;
		unsigned __int64 decalSurfSort : 6;
		unsigned __int64 earlyDepthSort : 7;
	};
	
	union GfxSortKey
	{
		GfxDrawSurfFields fields;
		uint64_t packed;
	};
	
	struct MaterialInfo
	{
		const char *name;
		unsigned int namehash;
		unsigned int gameFlags;
		char layerSortDecal;
		char pad;
		char textureAtlasRowCount;
		char textureAtlasColumnCount;
		GfxSortKey drawSurf;
		unsigned int hashIndex;
		int surfaceFlags;
		unsigned int contents;
	};
	
	struct MaterialTechniqueSet
	{
		const char *name;
		unsigned int renderFlags;
		void *techniques[16];
	};
	
	struct Material
	{
		MaterialInfo info;
		char textureCount;
		char samplerCount;
		char constantCount;
		char cameraRegion; 
		char pad[0x240];
		MaterialTechniqueSet *techniqueSet;
	};

	union XAssetHeader
	{
		void *physPreset;
		void *physConstraints;
		void *destructibleDef;
		void *parts;
		void *model;
		void *modelMesh;
		void *material;
		void *computeShaderSet;
		void *techniqueSet;
		void *image;
		void *sound;
		void *clipMap;
		void *comWorld;
		void *gameWorld;
		void *gfxWorld;
		void *lensFlareDef;
		void *fontIcon;
		void *localize;
		void *localizeList;
		void *weapon;
		void *attachment;
		void *attachmentUnique;
		void *weaponCamo;
		void *customizationTable;
		void *customizationTable_feimages;
		void *customizationColorInfo;
		void *sndDriverGlobals;
		void *fx;
		void *tagFX;
		void* newLensFlareDef;
		void *impactFx;
		void *impactSounds;
		void *cgMedia;
		void *playerSounds;
		void *playerFX;
		void *sharedWeaponSounds;
		void *rawfile;
		void *stringTable;
		void *structuredTable;
		void *leaderboardDef;
		void *ddlRoot;
		void *glasses;
		void *scriptParseTree;
		void *scriptParseTreeDBG;
		void *keyValuePairs;
		void *vehicleDef;
		void *tracerDef;
		void *surfaceFXTableDef;
		void *surfaceSoundDef;
		void *footstepTableDef;
		void *entitySoundImpacts;
		void *entityFxImpacts;
		void *zbarrierDef;
		void *vehicleFxDef;
		void *vehicleSoundDef;
		void *typeInfo;
		void *scriptBundle;
		void *scriptBundleList;
		void *rumble;
		void *bulletPenetration;
		void *locDmgTable;
		void *aimTable;
		void *xModelAlias;
		void *character;
		void *aiType;
		void *animSelectorTableSet;
		void *animMappingTable;
		void *animStateMachine;
		void *behaviorTree;
		void *behaviorStateMachine;
		void *ttfDef;
		void *sanim;
		void *lightDescription;
		void *shellshock;
		void *cinematicCamera;
		void *xcam;
		void *bgCache;
		void *textureCombo;
		void *flameTable;
		void *bitfield;
		void *attachmentCosmeticVariant;
		void *mapTable;
		void *mapTableLoadingImages;
		void *mapTableEntryLevelAssets;
		void *medal;
		void *medalTable;
		void *objective;
		void *objectiveList;
		void *navMesh;
		void *navVolume;
		void *binaryHTML;
		void *laserDef;
		void *beamDef;
		void *streamerHint;
		void *flowGraphDef;
		void *postfxBundleDef;
		void *luafile;
		void *luafileDebug;
		void *renderOverrideBundleDef;
		void *staticLevelFxList;
		void *triggerList;
		void *playerRoleTemplate;
		void *playerRoleCategoryTable;
		void *playerRoleCategory;
		void *characterBodyType;
		void *gameTypeTable;
		void *unlockableItemTable;
		void *entityList;
		void *playlists;
		void *playlistGlobalSettings;
		void *playlistEvent;
		void *playlistEventSchedule;
		void *motionMatchingInputDef;
		void *blackBoardDef;
		void *tacticalQueryDef;
		void *pmoveDef;
		void *hierarchicalTaskNetwork;
		void *ragdollDef;
		void *storageFile;
		void *storageFileList;
		void *charMixer;
		void *data;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};

	struct cmd_function_s
	{
		cmd_function_s *next;
		const char *name;
		const char *autoCompleteDir;
		const char *autoCompleteExt;
		void(*function)();
	};

	struct Msg_ModifiedStats
	{
		int statsSize;
		byte statsBuffer[65536];
	};

	struct Msg_VoicePacket
	{
		int32_t lobbyType;
		uint8_t talkerIndex;
		int32_t relayBits;
		uint16_t sizeofVoiceData;
		uint8_t numPacketsInData;
		uint8_t voiceData[1198];
	};

	struct Msg_JoinAgreementRequest
	{
		uint64_t hostXuid;
		char hostName[32];
		bdSecurityID secId;
		bdSecurityKey secKey;
		SerializedAdr serializedAdr;
		int sourceLobbyType;
		int destinationLobbyType;
		LobbyParams destinationLobbyParams;
		uint64_t reservationKey;
		int agreementNonce;
		int serverLoc;
	};

	struct Msg_LobbyHostHeartbeat
	{
		int heartbeatNum;
		int lobbyType;
		MsgHostMigrateInfo migrateInfo;
	};

	struct SessionSearchThrottleData
	{
		int returnTimeMs;
	};

	struct SessionSearchPayloadData
	{
		bool(*abortCheckfunc)(void);
		SessionSearchThrottleData *throttleData;
	};

	struct MatchMakingQuery
	{
		char pad[0xBC];
	};

	struct LobbySearch
	{
		int state;
		int errorCode;
		int numResults;
		char pad[0x4];
		MatchMakingQuery info;
		SessionSearchPayloadData results;
		SessionSearchThrottleData throttle;
	};
}
