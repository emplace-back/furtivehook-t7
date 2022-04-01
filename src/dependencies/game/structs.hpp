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
			std::uint8_t addr[4];
			std::uint32_t inaddr;
		};

		std::uint16_t port;
		char pad2[0x1];

		const std::string to_string(bool include_port = false) const
		{
			return include_port ? utils::string::va("%u.%u.%u.%u:%u", addr[0], addr[1], addr[2], addr[3], port) : utils::string::va("%u.%u.%u.%u", addr[0], addr[1], addr[2], addr[3]);
		}
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
		XNADDR xnaddr;
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
	
	struct InfoProbeResult
	{
		bool isValid;
		uint64_t probedXuid;
		uint64_t hostXuid;
		char hostName[32];
		LobbyType destinationLobby;
		LobbyParams destinationLobbyParams;
		int destinationLobbyUIScreen;
		int natType;
		bdSecurityID secId;
		bdSecurityKey secKey;
		SerializedAdr serializedAdr;
	};
	
	struct InfoProbe
	{
		bool active;
		unsigned int nonce;
		uint64_t probedXuid;
		int actionId;
		int startTime;
		InfoProbeResult result;
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


	struct Msg_JoinParty
	{
		int targetLobby;
		int sourceLobby;
		int joinType;
		uint64_t probedXuid;
		int memberCount;
		char pad[0x248];
		int playlistID;
		int playlistVersion;
		int ffotdVersion;
		short networkMode;
		uint32_t netFieldChecksum;
		int protocolVersion;
		int changelist;
		int pingBand;
		uint32_t dlcBits;
		uint64_t joinNonce;
		bool isStarterPack;
		char password[32];
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
		int timeSinceUpdate;
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
		int lobbyType;
		uint8_t talkerIndex;
		int relayBits;
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

	enum bdLobbyErrorCode
	{
		BD_NO_ERROR = 0x0,
		BD_TOO_MANY_TASKS = 0x1,
		BD_NOT_CONNECTED = 0x2,
		BD_SEND_FAILED = 0x3,
		BD_HANDLE_TASK_FAILED = 0x4,
		BD_START_TASK_FAILED = 0x5,
		BD_RESULT_EXCEEDS_BUFFER_SIZE = 0x64,
		BD_ACCESS_DENIED = 0x65,
		BD_EXCEPTION_IN_DB = 0x66,
		BD_MALFORMED_TASK_HEADER = 0x67,
		BD_INVALID_ROW = 0x68,
		BD_EMPTY_ARG_LIST = 0x69,
		BD_PARAM_PARSE_ERROR = 0x6A,
		BD_PARAM_MISMATCHED_TYPE = 0x6B,
		BD_SERVICE_NOT_AVAILABLE = 0x6C,
		BD_CONNECTION_RESET = 0x6D,
		BD_INVALID_USER_ID = 0x6E,
		BD_LOBBY_PROTOCOL_VERSION_FAILURE = 0x6F,
		BD_LOBBY_INTERNAL_FAILURE = 0x70,
		BD_LOBBY_PROTOCOL_ERROR = 0x71,
		BD_LOBBY_FAILED_TO_DECODE_UTF8 = 0x72,
		BD_LOBBY_ASCII_EXPECTED = 0x73,
		BD_INVALID_CONTEXT = 0x74,
		BD_LOBBY_METHOD_DISABLED = 0x75,
		BD_ASYNCHRONOUS_ERROR = 0xC8,
		BD_STREAMING_COMPLETE = 0xC9,
		BD_MEMBER_NO_PROPOSAL = 0x12C,
		BD_TEAMNAME_ALREADY_EXISTS = 0x12D,
		BD_MAX_TEAM_MEMBERSHIPS_LIMITED = 0x12E,
		BD_MAX_TEAM_OWNERSHIPS_LIMITED = 0x12F,
		BD_NOT_A_TEAM_MEMBER = 0x130,
		BD_INVALID_TEAM_ID = 0x131,
		BD_INVALID_TEAM_NAME = 0x132,
		BD_NOT_A_TEAM_OWNER = 0x133,
		BD_NOT_AN_ADMIN_OR_OWNER = 0x134,
		BD_MEMBER_PROPOSAL_EXISTS = 0x135,
		BD_MEMBER_EXISTS = 0x136,
		BD_TEAM_FULL = 0x137,
		BD_VULGAR_TEAM_NAME = 0x138,
		BD_TEAM_USERID_BANNED = 0x139,
		BD_TEAM_EMPTY = 0x13A,
		BD_INVALID_TEAM_PROFILE_QUERY_ID = 0x13B,
		BD_TEAMNAME_TOO_SHORT = 0x13C,
		BD_UNIQUE_PROFILE_DATA_EXISTS_ALREADY = 0x13D,
		BD_APPLICATION_EXISTS = 0x13E,
		BD_APPLICATIONS_MAX_EXCEEDED = 0x13F,
		BD_PAGE_SIZE_LIMIT_EXCEEDED = 0x140,
		BD_APPLICATION_REJECTED_EXISTS = 0x141,
		BD_APPLICATION_WITHDRAWN_EXISTS = 0x142,
		BD_APPLICATION_DOES_NOT_EXIST = 0x143,
		BD_APPLICATION_INVALID_STATE = 0x144,
		BD_MEMBER_BAN_EXISTS = 0x145,
		BD_MEMBER_BAN_DOES_NOT_EXIST = 0x146,
		BD_OWNER_BAN_FORBIDDEN = 0x147,
		BD_INVALID_ACCOUNT_TYPE = 0x148,
		BD_CONFIGURED_ACCOUNT_TYPE_NOT_FOUND = 0x149,
		BD_OWNER_OF_NON_EMPTY_TEAM = 0x14A,
		BD_CANNOT_APPLY_TO_PIRIVATE_TEAM = 0x14B,
		BD_MEMBER_IS_OWNER = 0x15E,
		BD_AUTO_JOINING_DISABLED = 0x15F,
		BD_TEAM_SHOWCASE_DISABLED = 0x160,
		BD_INVALID_FILTER = 0x162,
		BD_INVALID_TEAM_LEADERBOARD = 0x163,
		BD_TEAM_LEADERBOARD_LOADING = 0x164,
		BD_TEAM_SHOWCASE_COUNT_EXCEEDED = 0x165,
		BD_USER_ALREADY_TEAM_OWNER = 0x16A,
		BD_INVALID_BASE_CONTEXT = 0x16C,
		BD_INVALID_LEADERBOARD_ID = 0x190,
		BD_INVALID_STATS_SET = 0x191,
		BD_EMPTY_STATS_SET_IGNORED = 0x193,
		BD_NO_DIRECT_ACCESS_TO_ARBITRATED_LBS = 0x194,
		BD_STATS_WRITE_PERMISSION_DENIED = 0x195,
		BD_STATS_WRITE_TYPE_DATA_TYPE_MISMATCH = 0x196,
		BD_NO_STATS_FOR_USER = 0x197,
		BD_INVALID_ACCESS_TO_UNRANKED_LB = 0x198,
		BD_INVALID_EXTERNAL_TITLE_ID = 0x199,
		BD_DIFFERENT_LEADERBOARD_SCHEMAS = 0x19A,
		BD_TOO_MANY_LEADERBOARDS_REQUESTED = 0x19B,
		BD_ENTITLEMENTS_ERROR = 0x19C,
		BD_ENTITLEMENTS_INVALID_TITLEID = 0x19D,
		BD_ENTITLEMENTS_INVALID_LEADERBOARDID = 0x19E,
		BD_ENTITLEMENTS_INVALID_GET_MODE_FOR_TITLE = 0x19F,
		BD_ENTITLEMENTS_URL_CONNECTION_ERROR = 0x1A0,
		BD_ENTITLEMENTS_CONFIG_ERROR = 0x1A1,
		BD_ENTITLEMENTS_NAMED_PARENT_ERROR = 0x1A2,
		BD_ENTITLEMENTS_NAMED_KEY_ERROR = 0x1A3,
		BD_TOO_MANY_ENTITY_IDS_REQUESTED = 0x1A4,
		BD_STATS_READ_FAILED = 0x1A5,
		BD_STATS_INVALID_WRITE_TO_VIRTUAL_LEADERBOARD = 0x1A6,
		BD_STATS_INVALID_WRITE_TYPE_TO_MULTIRANK_LB = 0x1A7,
		BD_STATS_INVALID_EXCLUDED_COLUMN = 0x1A8,
		BD_STATS_INVALID_INCLUDED_COLUMN = 0x1A9,
		BD_STATS_WRITE_NO_SCRIPT_MODULE = 0x1AA,
		BD_STATS_WRITE_SCRIPT_MODULE_ERROR = 0x1AB,
		BD_NON_READ_ON_READ_ONLY_LEADERBOARD = 0x1AC,
		BD_INVALID_TITLE_ID = 0x1F4,
		BD_DOWN_FOR_MAINTENANCE = 0x1F5,
		BD_MESSAGING_INVALID_MAIL_ID = 0x258,
		BD_SELF_BLOCK_NOT_ALLOWED = 0x259,
		BD_GLOBAL_MESSAGE_ACCESS_DENIED = 0x25A,
		BD_GLOBAL_MESSAGES_USER_LIMIT_EXCEEDED = 0x25B,
		BD_MESSAGING_SENDER_DOES_NOT_EXIST = 0x25C,
		BD_AUTH_NO_ERROR = 0x2BC,
		BD_AUTH_BAD_REQUEST = 0x2BD,
		BD_AUTH_SERVER_CONFIG_ERROR = 0x2BE,
		BD_AUTH_BAD_TITLE_ID = 0x2BF,
		BD_AUTH_BAD_ACCOUNT = 0x2C0,
		BD_AUTH_ILLEGAL_OPERATION = 0x2C1,
		BD_AUTH_INCORRECT_LICENSE_CODE = 0x2C2,
		BD_AUTH_CREATE_USERNAME_EXISTS = 0x2C3,
		BD_AUTH_CREATE_USERNAME_ILLEGAL = 0x2C4,
		BD_AUTH_CREATE_USERNAME_VULGAR = 0x2C5,
		BD_AUTH_CREATE_MAX_ACC_EXCEEDED = 0x2C6,
		BD_AUTH_MIGRATE_NOT_SUPPORTED = 0x2C7,
		BD_AUTH_TITLE_DISABLED = 0x2C8,
		BD_AUTH_ACCOUNT_EXPIRED = 0x2C9,
		BD_AUTH_ACCOUNT_LOCKED = 0x2CA,
		BD_AUTH_UNKNOWN_ERROR = 0x2CB,
		BD_AUTH_INCORRECT_PASSWORD = 0x2CC,
		BD_AUTH_IP_NOT_IN_ALLOWED_RANGE = 0x2CD,
		BD_AUTH_WII_TOKEN_VERIFICATION_FAILED = 0x2CE,
		BD_AUTH_WII_AUTHENTICATION_FAILED = 0x2CF,
		BD_AUTH_IP_KEY_LIMIT_REACHED = 0x2D0,
		BD_AUTH_INVALID_GSPID = 0x2D1,
		BD_AUTH_INVALID_IP_RANGE_ID = 0x2D2,
		BD_AUTH_3DS_TOKEN_VERIFICATION_FAILED = 0x2D1,
		BD_AUTH_3DS_AUTHENTICATION_FAILED = 0x2D2,
		BD_AUTH_STEAM_APP_ID_MISMATCH = 0x2D3,
		BD_AUTH_ABACCOUNTS_APP_ID_MISMATCH = 0x2D4,
		BD_AUTH_CODO_USERNAME_NOT_SET = 0x2D5,
		BD_AUTH_WIIU_TOKEN_VERIFICATION_FAILED = 0x2D6,
		BD_AUTH_WIIU_AUTHENTICATION_FAILED = 0x2D7,
		BD_AUTH_CODO_USERNAME_NOT_BASE64 = 0x2D8,
		BD_AUTH_CODO_USERNAME_NOT_UTF8 = 0x2D9,
		BD_AUTH_TENCENT_TICKET_EXPIRED = 0x2DA,
		BD_AUTH_PS3_SERVICE_ID_MISMATCH = 0x2DB,
		BD_AUTH_CODOID_NOT_WHITELISTED = 0x2DC,
		BD_AUTH_PLATFORM_TOKEN_RETRIEVAL_ERROR = 0x2DD,
		BD_AUTH_JSON_FORMAT_ERROR = 0x2DE,
		BD_AUTH_REPLY_CONTENT_ERROR = 0x2DF,
		BD_AUTH_PLATFORM_TOKEN_EXPIRED = 0x2E0,
		BD_AUTH_CONTINUING = 0x2E1,
		BD_AUTH_PLATFORM_TOKEN_DECRYPTION_ERROR = 0x2E2,
		BD_AUTH_PLATFORM_TOKEN_SIGNATURE_ERROR = 0x2E3,
		BD_AUTH_DNS_RESOLUTION_ERROR = 0x2E4,
		BD_AUTH_SSL_CERTIFICATE_ERROR = 0x2E5,
		BD_AUTH_SERVER_UNAVAILABLE_ERROR = 0x2E6,
		BD_AUTH_ENVIRONMENT_ERROR = 0x2E7,
		BD_AUTH_PLATFORM_DEVICE_ID_ERROR = 0x2E8,
		BD_AUTH_UNO_APP_ID_MISMATCH = 0x2E9,
		BD_AUTH_UNICODE_DECODE_ERROR = 0x2EA,
		BD_AUTH_STEAM_PUBLISHER_BAN = 0x2EB,
		BD_AUTH_TICKET_DECRYPTION_ERROR = 0x2EC,
		BD_AUTH_SIGNATURE_ERROR = 0x2ED,
		BD_AUTH_REQUEST_TIMEOUT_ERROR = 0x2EE,
		BD_AUTH_REQUEST_ABORTED_ERROR = 0x2EF,
		BD_AUTH_SINGLE_IDENTITY_FLOW_DISABLED_ERROR = 0x2F0,
		BD_AUTH_SINGLE_IDENTITY_TOKEN_MISSING_ERROR = 0x2F1,
		BD_AUTH_SINGLE_IDENTITY_TOKEN_EXPIRED_ERROR = 0x2F2,
		BD_AUTH_SINGLE_IDENTITY_TOKEN_INVALID_ERROR = 0x2F3,
		BD_NO_PROFILE_INFO_EXISTS = 0x320,
		BD_FRIENDSHIP_NOT_REQUSTED = 0x384,
		BD_NOT_A_FRIEND = 0x385,
		BD_SELF_FRIENDSHIP_NOT_ALLOWED = 0x387,
		BD_FRIENDSHIP_EXISTS = 0x388,
		BD_PENDING_FRIENDSHIP_EXISTS = 0x389,
		BD_USERID_BANNED = 0x38A,
		BD_FRIENDS_FULL = 0x38C,
		BD_FRIENDS_NO_RICH_PRESENCE = 0x38D,
		BD_RICH_PRESENCE_TOO_LARGE = 0x38E,
		BD_NO_FILE = 0x3E8,
		BD_PERMISSION_DENIED = 0x3E9,
		BD_FILESIZE_LIMIT_EXCEEDED = 0x3EA,
		BD_FILENAME_MAX_LENGTH_EXCEEDED = 0x3EB,
		BD_EXTERNAL_STORAGE_SERVICE_ERROR = 0x3EC,
		BD_VALIDATION_ERROR = 0x3ED,
		BD_VALIDATION_TOKEN_ERROR = 0x3EE,
		BD_VALIDATION_TOKEN_GENERATION_ERROR = 0x3EF,
		BD_VALIDATION_TOKEN_VERIFICATION_ERROR = 0x3F0,
		BD_STORAGE_SERVER_UNREACHABLE = 0x3F1,
		BD_CHANNEL_DOES_NOT_EXIST = 0x44D,
		BD_CHANNEL_ALREADY_SUBSCRIBED = 0x44E,
		BD_CHANNEL_NOT_SUBSCRIBED = 0x44F,
		BD_CHANNEL_FULL = 0x450,
		BD_CHANNEL_SUBSCRIPTIONS_FULL = 0x451,
		BD_CHANNEL_NO_SELF_WHISPERING = 0x452,
		BD_CHANNEL_ADMIN_REQUIRED = 0x453,
		BD_CHANNEL_TARGET_NOT_SUBSCRIBED = 0x454,
		BD_CHANNEL_REQUIRES_PASSWORD = 0x455,
		BD_CHANNEL_TARGET_IS_SELF = 0x456,
		BD_CHANNEL_PUBLIC_BAN_NOT_ALLOWED = 0x457,
		BD_CHANNEL_USER_BANNED = 0x458,
		BD_CHANNEL_PUBLIC_PASSWORD_NOT_ALLOWED = 0x459,
		BD_CHANNEL_PUBLIC_KICK_NOT_ALLOWED = 0x45A,
		BD_CHANNEL_MUTED = 0x45B,
		BD_CONTENT_UNLOCK_UNKNOWN_ERROR = 0x514,
		BD_UNLOCK_KEY_INVALID = 0x515,
		BD_UNLOCK_KEY_ALREADY_USED_UP = 0x516,
		BD_CONTENT_UNLOCK_LIMIT_REACHED = 0x517,
		BD_DIFFERENT_HARDWARE_ID = 0x518,
		BD_INVALID_CONTENT_OWNER = 0x519,
		BD_CONTENT_UNLOCK_INVALID_USER = 0x51A,
		BD_CONTENT_UNLOCK_INVALID_CATEGORY = 0x51B,
		BD_KEY_ARCHIVE_INVALID_WRITE_TYPE = 0x5DC,
		BD_KEY_ARCHIVE_EXCEEDED_MAX_IDS_PER_REQUEST = 0x5DD,
		BD_BANDWIDTH_TEST_TRY_AGAIN = 0x712,
		BD_BANDWIDTH_TEST_STILL_IN_PROGRESS = 0x713,
		BD_BANDWIDTH_TEST_NOT_PROGRESS = 0x714,
		BD_BANDWIDTH_TEST_SOCKET_ERROR = 0x715,
		BD_INVALID_SESSION_NONCE = 0x76D,
		BD_ARBITRATION_FAILURE = 0x76F,
		BD_ARBITRATION_USER_NOT_REGISTERED = 0x771,
		BD_ARBITRATION_NOT_CONFIGURED = 0x772,
		BD_CONTENTSTREAMING_FILE_NOT_AVAILABLE = 0x7D0,
		BD_CONTENTSTREAMING_STORAGE_SPACE_EXCEEDED = 0x7D1,
		BD_CONTENTSTREAMING_NUM_FILES_EXCEEDED = 0x7D2,
		BD_CONTENTSTREAMING_UPLOAD_BANDWIDTH_EXCEEDED = 0x7D3,
		BD_CONTENTSTREAMING_FILENAME_MAX_LENGTH_EXCEEDED = 0x7D4,
		BD_CONTENTSTREAMING_MAX_THUMB_DATA_SIZE_EXCEEDED = 0x7D5,
		BD_CONTENTSTREAMING_DOWNLOAD_BANDWIDTH_EXCEEDED = 0x7D6,
		BD_CONTENTSTREAMING_NOT_ENOUGH_DOWNLOAD_BUFFER_SPACE = 0x7D7,
		BD_CONTENTSTREAMING_SERVER_NOT_CONFIGURED = 0x7D8,
		BD_CONTENTSTREAMING_INVALID_APPLE_RECEIPT = 0x7DA,
		BD_CONTENTSTREAMING_APPLE_STORE_NOT_AVAILABLE = 0x7DB,
		BD_CONTENTSTREAMING_APPLE_RECEIPT_FILENAME_MISMATCH = 0x7DC,
		BD_CONTENTSTREAMING_BATCH_DOWNLOAD_PARTIAL_FAILURE = 0x7DD,
		BD_CONTENTSTREAMING_HTTP_ERROR = 0x7E4,
		BD_CONTENTSTREAMING_FAILED_TO_START_HTTP = 0x7E5,
		BD_CONTENTSTREAMING_LOCALE_INVALID = 0x7E6,
		BD_CONTENTSTREAMING_LOCALE_MISSING = 0x7E7,
		BD_VOTERANK_ERROR_EMPTY_RATING_SUBMISSION = 0x7EE,
		BD_VOTERANK_ERROR_MAX_VOTES_EXCEEDED = 0x7EF,
		BD_VOTERANK_ERROR_INVALID_RATING = 0x7F0,
		BD_MAX_NUM_TAGS_EXCEEDED = 0x82A,
		BD_TAGGED_COLLECTION_DOES_NOT_EXIST = 0x82B,
		BD_EMPTY_TAG_ARRAY = 0x82C,
		BD_SEARCH_SERVER_UNREACHABLE = 0x82F,
		BD_ENTITY_ID_NOT_SPECIFIED = 0x830,
		BD_INVALID_QUERY_ID = 0x834,
		BD_NO_ENTRY_TO_UPDATE = 0x835,
		BD_SESSION_INVITE_EXISTS = 0x836,
		BD_INVALID_SESSION_ID = 0x837,
		BD_ATTACHMENT_TOO_LARGE = 0x838,
		BD_INVALID_FIELD_VALUE = 0x839,
		BD_INVALID_GROUP_ID = 0xAF0,
		BD_MAIL_INVALID_MAIL_ID_ERROR = 0xB55,
		BD_MAIL_PERMISSION_DENIED_ERROR = 0xB56,
		BD_TWITCH_SERVICE_ERROR = 0xC1D,
		BD_TWITCH_ACCOUNT_ALREADY_LINKED = 0xC1E,
		BD_TWITCH_NO_LINKED_ACCOUNT = 0xC1F,
		BD_TWITTER_AUTH_ATTEMPT_FAILED = 0xDAD,
		BD_TWITTER_AUTH_TOKEN_INVALID = 0xDAE,
		BD_TWITTER_UPDATE_LIMIT_REACHED = 0xDAF,
		BD_TWITTER_UNAVAILABLE = 0xDB0,
		BD_TWITTER_ERROR = 0xDB1,
		BD_TWITTER_TIMED_OUT = 0xDB2,
		BD_TWITTER_ACCOUNT_AMBIGUOUS = 0xDB4,
		BD_TWITTER_MAXIMUM_ACCOUNTS_REACHED = 0xDB5,
		BD_TWITTER_ACCOUNT_NOT_REGISTERED = 0xDB6,
		BD_TWITTER_DUPLICATE_STATUS = 0xDB7,
		BD_TWITTER_ACCOUNT_ALREADY_REGISTERED = 0xE1C,
		BD_FACEBOOK_AUTH_ATTEMPT_FAILED = 0xE11,
		BD_FACEBOOK_AUTH_TOKEN_INVALID = 0xE12,
		BD_FACEBOOK_PHOTO_DOES_NOT_EXIST = 0xE13,
		BD_FACEBOOK_PHOTO_INVALID = 0xE14,
		BD_FACEBOOK_PHOTO_ALBUM_FULL = 0xE15,
		BD_FACEBOOK_UNAVAILABLE = 0xE16,
		BD_FACEBOOK_ERROR = 0xE17,
		BD_FACEBOOK_TIMED_OUT = 0xE18,
		BD_FACEBOOK_DISABLED_FOR_USER = 0xE19,
		BD_FACEBOOK_ACCOUNT_AMBIGUOUS = 0xE1A,
		BD_FACEBOOK_MAXIMUM_ACCOUNTS_REACHED = 0xE1B,
		BD_FACEBOOK_INVALID_NUM_PICTURES_REQUESTED = 0xE1C,
		BD_FACEBOOK_VIDEO_DOES_NOT_EXIST = 0xE1D,
		BD_FACEBOOK_ACCOUNT_ALREADY_REGISTERED = 0xE1E,
		BD_FACEBOOK_TARGET_OBJECT_ID_INVALID = 0xE1F,
		BD_FACEBOOK_NO_SUCH_ACCOUNT = 0xE20,
		BD_APNS_INVALID_PAYLOAD = 0xE74,
		BD_APNS_INVALID_TOKEN_LENGTH_ERROR = 0xE76,
		BD_MAX_CONSOLEID_LENGTH_EXCEEDED = 0xEE1,
		BD_MAX_WHITELIST_LENGTH_EXCEEDED = 0xEE2,
		BD_TOTP_CHALLENGE_FAILED = 0xEE3,
		BD_NO_TOTP_ACCOUNT = 0xEE4,
		BD_EXTENDED_AUTH_INFO_ERROR = 0xEE5,
		BD_EXTENDED_AUTH_INFO_LENGTH_EXCEEDED = 0xEE6,
		BD_EXTENDED_AUTH_INFO_EXPIRED = 0xEE7,
		BD_WHITELIST_TRAILING_WHITE_SPACE = 0xEE8,
		BD_USERGROUP_NAME_ALREADY_EXISTS = 0x1770,
		BD_INVALID_USERGROUP_ID = 0x1771,
		BD_USER_ALREADY_IN_USERGROUP = 0x1772,
		BD_USER_NOT_IN_USERGROUP = 0x1773,
		BD_INVALID_USERGROUP_MEMBER_TYPE = 0x1774,
		BD_TOO_MANY_MEMBERS_REQUESTED = 0x1775,
		BD_USERGROUP_NAME_TOO_SHORT = 0x1776,
		BD_RICH_PRESENCE_DATA_TOO_LARGE = 0x1A90,
		BD_RICH_PRESENCE_TOO_MANY_USERS = 0x1A91,
		BD_PRESENCE_DATA_TOO_LARGE = 0x283C,
		BD_PRESENCE_TOO_MANY_USERS = 0x283D,
		BD_USER_LOGGED_IN_OTHER_TITLE = 0x283E,
		BD_USER_NOT_LOGGED_IN = 0x283F,
		BD_PRESENCE_INVALID_PLATFORM = 0x2840,
		BD_SUBSCRIPTION_TOO_MANY_USERS = 0x1B58,
		BD_SUBSCRIPTION_TICKET_PARSE_ERROR = 0x1B59,
		BD_CODO_ID_INVALID_DATA = 0x1BBC,
		BD_INVALID_MESSAGE_FORMAT = 0x1BBD,
		BD_TLOG_TOO_MANY_MESSAGES = 0x1BBE,
		BD_CODO_ID_NOT_IN_WHITELIST = 0x1BBF,
		BD_TLOG_MESSAGE_TRANSFORMATION_ERROR = 0x1BC0,
		BD_REWARDS_NOT_ENABLED = 0x1BC1,
		BD_REWARDS_INVALID_RULE = 0x1BC2,
		BD_REDEEM_NAME_CHANGE_INTERNAL_ERROR = 0x1BC3,
		BD_REDEEM_NAME_CHANGE_INVALID_ITEM = 0x1BC4,
		BD_REDEEM_NAME_CHANGE_UNAVAILABLE_ITEM = 0x1BC5,
		BD_REDEEM_NAME_CHANGE_IN_PROGRESS = 0x1BC6,
		BD_REDEEM_NAME_CHANGE_INACTIVE_REQUEST = 0x1BC7,
		BD_REDEEM_NAME_CHANGE_INVALID_NAME = 0x1BC8,
		BD_REDEEM_NAME_CHANGE_SAME_NAME = 0x1BC9,
		BD_REDEEM_NAME_CHANGE_DUPLICATE_NAME = 0x1BCA,
		BD_TENCENT_NO_APPLICABLE_REWARDS = 0x1BCB,
		BD_TLOG_MESSAGE_DECOMPRESSION_ERROR = 0x1BCC,
		BD_REDEEM_TEAM_NAME_CHANGE_INTERNAL_ERROR = 0x1BCD,
		BD_REDEEM_TEAM_NAME_CHANGE_INVALID_ITEM = 0x1BCE,
		BD_REDEEM_TEAM_NAME_CHANGE_UNAVAILABLE_ITEM = 0x1BCF,
		BD_MARKETPLACE_ERROR = 0x1F40,
		BD_MARKETPLACE_RESOURCE_NOT_FOUND = 0x1F41,
		BD_MARKETPLACE_INVALID_CURRENCY = 0x1F42,
		BD_MARKETPLACE_INVALID_PARAMETER = 0x1F43,
		BD_MARKETPLACE_RESOURCE_CONFLICT = 0x1F44,
		BD_MARKETPLACE_STORAGE_ERROR = 0x1F45,
		BD_MARKETPLACE_INTEGRITY_ERROR = 0x1F46,
		BD_MARKETPLACE_INSUFFICIENT_FUNDS_ERROR = 0x1F47,
		BD_MARKETPLACE_MMP_SERVICE_ERROR = 0x1F48,
		BD_MARKETPLACE_PRECONDITION_REQUIRED = 0x1F49,
		BD_MARKETPLACE_ITEM_MULTIPLE_PURCHASE_ERROR = 0x1F4A,
		BD_MARKETPLACE_MISSING_REQUIRED_ENTITLEMENT = 0x1F4B,
		BD_MARKETPLACE_VALIDATION_ERROR = 0x1F4C,
		BD_MARKETPLACE_TENCENT_PAYMENT_ERROR = 0x1F4D,
		BD_MARKETPLACE_SKU_NOT_COUPON_ENABLED_ERROR = 0x1F4E,
		BD_MARKETPLACE_TRANSACTION_ERROR = 0x1F4F,
		BD_MARKETPLACE_RECEIPT_USED = 0x1F50,
		BD_MARKETPLACE_INVALID_RECEIPT = 0x1F51,
		BD_MARKETPLACE_STEAM_REQUEST_FAILED = 0x1F52,
		BD_MARKETPLACE_STEAM_NOT_APPROVED = 0x1F53,
		BD_MARKETPLACE_PRODUCT_NOT_FOUND_IN_FP = 0x1F54,
		BD_MARKETPLACE_STEAM_ALREADY_FINALIZED = 0x1F54,
		BD_MARKETPLACE_STEAM_BAD_ORDER_ID = 0x1F55,
		BD_MARKETPLACE_STEAM_CURRENCY_ERROR = 0x1F56,
		BD_MARKETPLACE_PSN_INVALID_NP_TITLE_ID = 0x1F65,
		BD_MARKETPLACE_PSN_INVALID_NP_AUTH_CODE = 0x1F57,
		BD_MARKETPLACE_PSN_INVALID_ENTITLEMENT_ID = 0x1F58,
		BD_MARKETPLACE_PSN_ENTITLEMENT_NOT_PRESENT = 0x1F59,
		BD_MARKETPLACE_PSN_INTERNAL_ERROR = 0x1F5A,
		BD_MARKETPLACE_XB1_UNAUTHORISED_ERROR = 0x1F5B,
		BD_MARKETPLACE_XB1_REQUEST_REJECTED_ERROR = 0x1F5C,
		BD_MARKETPLACE_XB1_INSUFFICIENT_QUANTITY_ERROR = 0x1F5D,
		BD_MARKETPLACE_XB1_BAD_DELEGATION_TOKEN_ERROR = 0x1F5E,
		BD_MARKETPLACE_XB1_REQUEST_FAILED_ERROR = 0x1F5F,
		BD_MARKETPLACE_X360_REQUEST_FAILED = 0x1F60,
		BD_MARKETPLACE_X360_ITEM_NOT_PRESENT = 0x1F61,
		BD_MARKETPLACE_X360_CONNECTION_ERROR = 0x1F62,
		BD_MARKETPLACE_USAGE_TIME_ERROR = 0x1F63,
		BD_MARKETPLACE_DURABLE_ALREADY_GRANTED = 0x1F64,
		BD_MARKETPLACE_FIRST_PARTY_DURABLE_EXISTS = 0x1F66,
		BD_MARKETPLACE_ITEM_LIMIT_REACHED = 0x1F67,
		BD_MARKETPLACE_OVER_ITEM_MAX_QUANTITY_ERROR = 0x1F69,
		BD_MARKETPLACE_INSUFFICIENT_ITEM_QUANTITY = 0x1F6A,
		BD_MARKETPLACE_ENTITY_NOT_ENABLED = 0x1F6B,
		BD_MARKETPLACE_MISCONFIGURED = 0x1F6C,
		BD_MARKETPLACE_COUPON_NOT_CLAIMED_OR_FOUND = 0x1F6D,
		BD_MARKETPLACE_INVALID_DISCOUNT = 0x1F6E,
		BD_MARKETPLACE_INVALID_STORE_VERSION = 0x1F6F,
		BD_MARKETPLACE_SKU_SOLD_OUT = 0x1F72,
		BD_MARKETPLACE_WIIU_AUTH_FAILED = 0x1F73,
		BD_MARKETPLACE_WIIU_INVALID_PARAMETER = 0x1F74,
		BD_MARKETPLACE_WIIU_TAX_LOCATION_NOT_SPECIFIED = 0x1F75,
		BD_MARKETPLACE_WIIU_ACCOUNT_ERROR = 0x1F76,
		BD_MARKETPLACE_WIIU_PURCHASING_ERROR = 0x1F77,
		BD_MARKETPLACE_WIIU_BALANCE_ERROR = 0x1F78,
		BD_MARKETPLACE_WIIU_SERVER_ERROR = 0x1F79,
		BD_MARKETPLACE_WIIU_REQUEST_FAILED = 0x1F7A,
		BD_MARKETPLACE_WIIU_SERVER_MAINTENANCE = 0x1F7B,
		BD_MARKETPLACE_WIIU_SERVICE_TERMINATED = 0x1F7C,
		BD_MARKETPLACE_WIIU_ITEM_NOT_PRESENT = 0x1F7D,
		BD_MARKETPLACE_WIIU_TAX_LOCATION_INVALID = 0x1F7E,
		BD_MARKETPLACE_XB1_DURABLE_NOT_PRESENT = 0x1F7F,
		BD_MARKETPLACE_EXPECTED_PRICE_MISMATCH = 0x1F80,
		BD_MARKETPLACE_ITEM_NOT_CONSUMABLE = 0x1F81,
		BD_MARKETPLACE_IDEMPOTENT_REQUEST_COLLISION = 0x1F82,
		BD_MARKETPLACE_COUPON_NOT_STARTED = 0x1F83,
		BD_MARKETPLACE_MULTIPLE_OPERATIONS_ERROR = 0x1F84,
		BD_MARKETPLACE_MISSING_PAYMENT_PROVIDER_CURRENCY_ERROR = 0x1F85,
		BD_MARKETPLACE_WIIU_LANGUAGE_NOT_SUPPORTED = 0x1F86,
		BD_MARKETPLACE_PAWN_CHOICE_EXPECTED = 0x1F87,
		BD_MARKETPLACE_PAWN_CHOICE_UNSUPPORTED = 0x1F88,
		BD_MARKETPLACE_INVALID_REWARD_ERROR = 0x1F89,
		BD_COMMS_SERVICE_NOT_AVAILABLE_ERROR = 0x28A0,
		BD_COMMS_CLIENT_ERROR = 0x28A1,
		BD_LEAGUE_INVALID_TEAM_SIZE = 0x1FA4,
		BD_LEAGUE_INVALID_TEAM = 0x1FA5,
		BD_LEAGUE_INVALID_SUBDIVISION = 0x1FA6,
		BD_LEAGUE_INVALID_LEAGUE = 0x1FA7,
		BD_LEAGUE_TOO_MANY_RESULTS_REQUESTED = 0x1FA8,
		BD_LEAGUE_METADATA_TOO_LARGE = 0x1FA9,
		BD_LEAGUE_TEAM_ICON_TOO_LARGE = 0x1FAA,
		BD_LEAGUE_TEAM_NAME_TOO_LONG = 0x1FAB,
		BD_LEAGUE_ARRAY_SIZE_MISMATCH = 0x1FAC,
		BD_LEAGUE_SUBDIVISION_MISMATCH = 0x2008,
		BD_LEAGUE_INVALID_WRITE_TYPE = 0x2009,
		BD_LEAGUE_INVALID_STATS_DATA = 0x200A,
		BD_LEAGUE_SUBDIVISION_UNRANKED = 0x200B,
		BD_LEAGUE_CROSS_TEAM_STATS_WRITE_PREVENTED = 0x200C,
		BD_LEAGUE_INVALID_STATS_SEASON = 0x200D,
		BD_COMMERCE_ERROR = 0x206C,
		BD_COMMERCE_RESOURCE_NOT_FOUND = 0x206D,
		BD_COMMERCE_STORAGE_INVALID_PARAMETER = 0x206E,
		BD_COMMERCE_APPLICATION_INVALID_PARAMETER = 0x206F,
		BD_COMMERCE_RESOURCE_CONFLICT = 0x2070,
		BD_COMMERCE_STORAGE_ERROR = 0x2071,
		BD_COMMERCE_INTEGRITY_ERROR = 0x2072,
		BD_COMMERCE_MMP_SERVICE_ERROR = 0x2073,
		BD_COMMERCE_PERMISSION_DENIED = 0x2074,
		BD_COMMERCE_INSUFFICIENT_FUNDS_ERROR = 0x2075,
		BD_COMMERCE_UNKNOWN_CURRENCY = 0x2076,
		BD_COMMERCE_INVALID_RECEIPT = 0x2077,
		BD_COMMERCE_RECEIPT_USED = 0x2078,
		BD_COMMERCE_TRANSACTION_ALREADY_APPLIED = 0x2079,
		BD_COMMERCE_INVALID_CURRENCY_TYPE = 0x207A,
		BD_COMMERCE_UNKNOWN_PLATFORM = 0x207B,
		BD_COMMERCE_WINDOWS_STORE_CERT_INVALID = 0x207C,
		BD_CONNECTION_COUNTER_ERROR = 0x20D0,
		BD_LINKED_ACCOUNTS_INVALID_CONTEXT = 0x2198,
		BD_LINKED_ACCOUNTS_INVALID_PLATFORM = 0x2199,
		BD_LINKED_ACCOUNTS_LINKED_ACCOUNTS_FETCH_ERROR = 0x219A,
		BD_LINKED_ACCOUNTS_INVALID_ACCOUNT = 0x219B,
		BD_LINKED_ACCOUNTS_INVALID_TOKEN = 0x219C,
		BD_QUEUED_MATCHING_ERROR = 0x2260,
		BD_QUEUED_MATCHING_NOT_FOUND = 0x2261,
		BD_QUEUED_MATCHING_WRONG_QUEUE_TYPE = 0x2262,
		BD_QUEUED_MATCHING_INVALID_QUEUE_ID = 0x2263,
		BD_LOGIN_QUEUE_NO_ERROR = 0x22C4,
		BD_LOGIN_QUEUE_ENVIRONMENT_ERROR = 0x22C5,
		BD_LOGIN_QUEUE_UNKNOWN_ERROR = 0x22C6,
		BD_LOGIN_QUEUE_BAD_REQUEST = 0x22C7,
		BD_LOGIN_QUEUE_SERVER_UNAVAILABLE_ERROR = 0x22C8,
		BD_LOGIN_QUEUE_SSL_CERTIFICATE_ERROR = 0x22C9,
		BD_LOGIN_QUEUE_DNS_RESOLUTION_ERROR = 0x22CA,
		BD_LOGIN_QUEUE_JSON_FORMAT_ERROR = 0x22CB,
		BD_LOGIN_QUEUE_TICKET_PARSE_ERROR = 0x22CC,
		BD_LOGIN_QUEUE_INVALID_TITLE_ID = 0x22CD,
		BD_VOTING_MAX_VALUE_EXCEEDED = 0x2328,
		BD_VOTING_INVALID_GROUP_NAME = 0x2329,
		BD_VOTING_IN_PROGRESS = 0x232A,
		BD_VOTING_NON_EXISTENT_GROUP = 0x232B,
		BD_USER_GENERATED_STATS_ERROR = 0x238C,
		BD_INVALID_ACCESS_TO_USER_GENERATED_LB_ERROR = 0x238D,
		BD_INVALID_ACCESS_TO_NON_USER_GENERATED_LB_ERROR = 0x238E,
		BD_SUB_LEADERBOARD_ID_MISMATCH_ERROR = 0x238D,
		BD_PUBLISHER_VARIABLES_SERVICE_ERROR = 0x251C,
		BD_PUBLISHER_VARIABLES_NOT_FOUND = 0x251D,
		BD_PUBLISHER_VARIABLES_INVALID_NAMESPACE = 0x251E,
		BD_PUBLISHER_VARIABLES_INVALID_GROUP_ID = 0x251F,
		BD_GMSG_INVALID_CATEGORY_ID = 0x27D8,
		BD_GMSG_CATEGORY_MEMBERSHIPS_LIMIT = 0x27D9,
		BD_GMSG_NONMEMBER_POST_DISALLOWED = 0x27DA,
		BD_GMSG_CATEGORY_DISALLOWS_CLIENT_TYPE = 0x27DB,
		BD_GMSG_PAYLOAD_TOO_BIG = 0x27DC,
		BD_GMSG_MEMBER_POST_DISALLOWED = 0x27DD,
		BD_GMSG_OVERLOADED = 0x27DE,
		BD_GMSG_USER_PERCATEGORY_POST_RATE_EXCEEDED = 0x27DF,
		BD_GMSG_USER_GLOBAL_POST_RATE_EXCEEDED = 0x27E0,
		BD_GMSG_GROUP_POST_RATE_EXCEEDED = 0x27E1,
		BD_GMSG_GROUP_LIMIT_REACHED = 0x27E2,
		BD_GMSG_GROUP_MEMBERSHIP_LIMIT_REACHED = 0x27E3,
		BD_SERVER_INVENTORY_SERVER_ERROR = 0x2904,
		BD_SERVER_INVENTORY_SERVER_ALLOCATED = 0x2905,
		BD_SERVER_INVENTORY_ENTRY_DOES_NOT_EXIST = 0x2906,
		BD_SERVER_INVENTORY_DATA_LAYER_ERROR = 0x2907,
		BD_SERVER_INVENTORY_NOT_ENABLED = 0x2908,
		BD_SERVER_INVENTORY_NO_SERVER_ALLOCATED = 0x2909,
		BD_SERVER_INVENTORY_MAX_REGISTRATION_LENGTH_EXCEEDED = 0x290A,
		BD_CODO_CLAN_NO_TEMP_RESERVATION = 0x2AF8,
		BD_CODO_CLAN_TOO_MANY_MEMBERSHIPS = 0x2AF9,
		BD_CODO_CLAN_UNEXPECTED_FOUNDATION_STATE = 0x2AFA,
		BD_CODO_CLAN_UNABLE_TO_FIND_NAME = 0x2AFB,
		BD_CODO_CLAN_INTERNAL_MARKETPLACE_ERROR = 0x2AFC,
		BD_CODO_CLAN_INSUFFICIENT_FUNDS = 0x2AFD,
		BD_CODO_CLAN_UNSATISFIED_PRECONDITION = 0x2AFE,
		BD_CODO_CLAN_NO_CLAN = 0x2AFF,
		BD_CODO_CLAN_NO_CHANNEL = 0x2B00,
		BD_CODO_CLAN_NOT_A_CLAN_MEMBER = 0x2B01,
		BD_CODO_CLAN_DISBAND_TOO_MANY_MEMBERS = 0x2B04,
		BD_CODO_CLAN_DISBAND_TOO_SOON = 0x2B05,
		BD_NO_SUCH_ROLE = 0x2EE0,
		BD_NO_SUCH_PERMISSION = 0x2EE1,
		BD_BAD_ROLE_SPECIFICATION = 0x2EE2,
		BD_BAD_PERMISSION_SPECIFICATION = 0x2EE3,
		BD_REDEEMABLE_CODE_REGISTRY_SERVICE_GENERIC_ERROR = 0x300D,
		BD_REDEEMABLE_CODE_MARKETPLACE_SERVICE_GENERIC_ERROR = 0x300E,
		BAD_PAYLOAD_ERROR = 0x300F,
		INVALID_PAYLOAD_SIGNATURE_ERROR = 0x3010,
		GENERATE_PAYLOAD_SIGNATURE_ERROR = 0x3011,
		BD_REDEEMABLE_CODE_EXPIRED_CLIENT_TRANSACTION_ID = 0x3012,
		BD_REDEEMABLE_CODE_NOT_FOUND = 0x3071,
		BD_REDEEMABLE_CODE_USE_LIMIT_EXCEEDED = 0x3072,
		BD_REDEEMABLE_CODE_ALREADY_USED_BY_USER = 0x3073,
		BD_REDEEMABLE_CODE_EXPIRED = 0x3074,
		INVALID_CLIENT_TRANSACTION_ID = 0x3075,
		BD_REWARD_NO_REWARD_TOKEN_FOUND = 0x364C,
		BD_REWARD_INVALID_ACHIEVEMENT_IDS_SPECIFIED = 0x364D,
		BD_REWARD_CLIENT_ACHIEVEMENTS_NOT_ENABLED = 0x364E,
		BD_REWARD_EVENTS_ERROR = 0x364F,
		BD_REWARD_EVENTS_NOT_ENABLED = 0x3650,
		BD_REWARD_EVENTS_RULES_ERROR = 0x3651,
		BD_REWARD_EVENTS_DATA_ERROR = 0x3652,
		BD_REWARD_EVENTS_TRANSACTION_ERROR = 0x3653,
		BD_REWARD_CONFIGURATION_ERROR = 0x3654,
		BD_REWARD_TOO_MANY_ACTIVE_CHALLENGES = 0x3655,
		BD_REWARD_CODO_TOO_MANY_REROLLS = 0x367E,
		BD_REWARD_CODO_BAD_REROLL_PARAMS = 0x367F,
		BD_UMBRELLA_PROVIDER_SERVER_ERROR = 0x36B0,
		BD_UMBRELLA_PROVIDER_UNSUPPORTED_OPERATION = 0x36B1,
		BD_UMBRELLA_ERROR = 0x36B2,
		BD_UMBRELLA_INVALID_TITLE_ID = 0x36B3,
		BD_UMBRELLA_INVALID_QUERY_STRING = 0x36B4,
		BD_UMBRELLA_INVALID_DATA = 0x36B5,
		BD_UMBRELLA_INVALID_CLIENT = 0x36B6,
		BD_UMBRELLA_PROVIDER_NOT_SUPPORTED = 0x36B7,
		BD_UMBRELLA_UNAUTHORIZED_ACCESS = 0x36B8,
		BD_UMBRELLA_INVALID_TOKEN = 0x36B8,
		BD_UMBRELLA_EXPIRED_PROVIDER_REFRESH_TOKEN = 0x36B9,
		BD_UMBRELLA_NO_VALID_PROVIDER_TOKENS = 0x36BA,
		BD_UMBRELLA_INVALID_LSG_TICKET = 0x36BB,
		BD_UMBRELLA_TOKEN_NOT_FOUND = 0x36BC,
		BD_UMBRELLA_USER_NOT_FOUND = 0x36BD,
		BD_UMBRELLA_ACCOUNT_NOT_FOUND = 0x36BE,
		BD_UMBRELLA_MERGE_CONFLICT = 0x36BF,
		BD_UMBRELLA_PROVIDER_INACCESSIBLE = 0x36C0,
		BD_UMBRELLA_MISSING_FIELD = 0x36C1,
		BD_UNO_ERROR = 0x3714,
		BD_UNO_INVALID_DATA = 0x3715,
		BD_UNO_INVALID_DATE_OF_BIRTH = 0x3716,
		BD_UNO_UNAUTHORIZED_ACCESS = 0x3717,
		BD_UNO_INVALID_TOKEN = 0x3718,
		BD_UNO_EXPIRED_TOKEN = 0x3719,
		BD_UNO_EMAIL_NOT_VERIFIED = 0x371A,
		BD_UNO_TOS_VERSION_NOT_FOUND = 0x371B,
		BD_UNO_TOS_CONTENT_NOT_FOUND = 0x371C,
		BD_UNO_TOS_ALREADY_ACCEPTED = 0x371D,
		BD_UNO_MISSING_FIELD = 0x371E,
		BD_UNO_DNS_RESOLUTION_FAILED = 0x371F,
		BD_SI_ERROR = 0x3777,
		BD_AMM_NOT_LOBBY_HOST = 0x38A4,
		BD_AMM_NOT_PARTY_HOST = 0x38A5,
		BD_AMM_NOT_LOBBY_MEMBER = 0x38A6,
		BD_AMM_INVALID_MATCHMAKING_ID = 0x38A7,
		BD_AMM_INVALID_LOBBY_ID = 0x38A8,
		BD_AMM_SEARCH_IN_PROGRESS = 0x38A9,
		BD_AMM_USER_ALREADY_MATCHMAKING = 0x38AA,
		BD_AMM_INVALID_TOKEN = 0x38AB,
		BD_AMM_INVALID_DOCUMENT_FORMAT = 0x38AC,
		BD_AMM_PLAYER_INFO_UNAVAILABLE = 0x38AD,
		BD_AMM_REQUEST_DESERIALIZATION_FAILED = 0x38AE,
		BD_AMM_INVALID_QOS_TRANSACTION_ID = 0x38AF,
		BD_AMM_INVALID_USER_ID_IN_GUEST_SLOTS = 0x38B0,
		BD_AMM_NO_BUILD_NAME_SET = 0x38B1,
		BD_AMM_LOBBY_MERGED = 0x38B2,
		BD_AMM_BACKOFF_REQUESTED = 0x38B3,
		BD_MATCH_SCHEDULER_ERROR = 0x3A98,
		BD_MATCH_SCHEDULER_SCHEDULED_MATCH_DOES_NOT_EXIST = 0x3A99,
		BD_MATCH_SCHEDULER_MAXIMUM_EVENTS_PER_MATCH_REACHED = 0x3A9A,
		BD_MLG_ERROR = 0x3AFC,
		BD_MLG_REMOTE_ERROR = 0x3AFD,
		BD_CODO_AUTO_JOIN_LEVEL_NOT_MET = 0x4A9D,
		BD_CODO_NOT_PERMITTED_TO_UPDATE_PROFILE_FIELD = 0x4A9E,
		BD_CODO_INVALID_PROFILE_VALUE = 0x4A9F,
		BD_CODO_PROFILE_COLUMN_DOES_NOT_EXIST = 0x4AA0,
		BD_CODO_LOUDSPEAKER_INVALID_TOKEN = 0x4AA1,
		BD_CODO_LOUDSPEAKER_INVALID_CATEGORY = 0x4AA2,
		BD_CODO_RESETTABLE_STATS_NOT_CONFIGURED = 0x4AA3,
		BD_MAX_ERROR_CODE = 0x4AA4,
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
		SearchState state;
		bdLobbyErrorCode errorCode;
		int numResults;
		char pad[0x4];
		MatchMakingQuery info;
		SessionSearchPayloadData results;
		SessionSearchThrottleData throttle;
	};

	enum PresenceActivity
	{
		PRESENCE_ACTIVITY_MENU_FIRST = 0x0,
		PRESENCE_ACTIVITY_UNKNOWN = 0x0,
		PRESENCE_ACTIVITY_OFFLINE = 0x0,
		PRESENCE_ACTIVITY_ONLINE_NOT_IN_TITLE = 0x1,
		PRESENCE_ACTIVITY_MENU_INLOBBY = 0x2,
		PRESENCE_ACTIVITY_MENU_LOOKING_FOR_PARTY = 0x3,
		PRESENCE_ACTIVITY_MENU_LOOKING_FOR_PLAYERS = 0x4,
		PRESENCE_ACTIVITY_MENU_LOCAL = 0x5,
		PRESENCE_ACTIVITY_MENU_START = 0x6,
		PRESENCE_ACTIVITY_IN_TITLE = 0x7,
		PRESENCE_ACTIVITY_NOT_IN_TITLE = 0x8,
		PRESENCE_ACTIVITY_MENU_LAST = 0xF,
		PRESENCE_ACTIVITY_CP_FIRST = 0x10,
		PRESENCE_ACTIVITY_CP_PLAYING_COOP_ON_MAP = 0x11,
		PRESENCE_ACTIVITY_CP_PLAYING_SOLO_ON_MAP = 0x12,
		PRESENCE_ACTIVITY_CP_PLAYING_COOP_IN_SAFEHOUSE = 0x13,
		PRESENCE_ACTIVITY_CP_PLAYING_SOLO_IN_SAFEHOUSE = 0x14,
		PRESENCE_ACTIVITY_CP_IN_LOBBY = 0x15,
		PRESENCE_ACTIVITY_CP_WATCHING_FILM = 0x16,
		PRESENCE_ACTIVITY_CP_LOOKING_FOR_PARTY = 0x17,
		PRESENCE_ACTIVITY_CP_LOOKING_FOR_PLAYERS = 0x18,
		PRESENCE_ACTIVITY_CP_PLAYING_LOCAL = 0x19,
		PRESENCE_ACTIVITY_CP_LAST = 0x1F,
		PRESENCE_ACTIVITY_MP_FIRST = 0x20,
		PRESENCE_ACTIVITY_MP_PLAYING_GMODE_ON_MAP = 0x21,
		PRESENCE_ACTIVITY_MP_IN_LOBBY = 0x22,
		PRESENCE_ACTIVITY_MP_WATCHING_FILM = 0x23,
		PRESENCE_ACTIVITY_MP_LOOKING_FOR_PARTY = 0x24,
		PRESENCE_ACTIVITY_MP_LOOKING_FOR_PLAYERS = 0x25,
		PRESENCE_ACTIVITY_MP_PLAYING_LOCAL = 0x26,
		PRESENCE_ACTIVITY_MP_LAST = 0x2F,
		PRESENCE_ACTIVITY_ZM_FIRST = 0x30,
		PRESENCE_ACTIVITY_ZM_PLAYING_MAP_ON_ROUND = 0x31,
		PRESENCE_ACTIVITY_ZM_IN_LOBBY = 0x32,
		PRESENCE_ACTIVITY_ZM_WATCHING_FILM = 0x33,
		PRESENCE_ACTIVITY_ZM_LOOKING_FOR_PARTY = 0x34,
		PRESENCE_ACTIVITY_ZM_LOOKING_FOR_PLAYERS = 0x35,
		PRESENCE_ACTIVITY_ZM_PLAYING_LOCAL = 0x36,
		PRESENCE_ACTIVITY_ZM_LAST = 0x3F,
		PRESENCE_ACTIVITY_LAST = 0x40,
	};

	enum PresenceCtx
	{
		PRESENCE_CTX_MENU_FIRST = 0x0,
		PRESENCE_CTX_UNKNOWN = 0x0,
		PRESENCE_CTX_MENU_MODE_SELECT = 0x1,
		PRESENCE_CTX_MENU_START = 0x2,
		PRESENCE_CTX_MENU_NOT_ACTIVE = 0x3,
		PRESENCE_CTX_MENU_LAST = 0xF,
		PRESENCE_CTX_CP_FIRST = 0x10,
		PRESENCE_CTX_CP_MAIN_MENU = 0x11,
		PRESENCE_CTX_CP_PRIVATE_GAME = 0x12,
		PRESENCE_CTX_CP_PUBLIC_MATCH = 0x13,
		PRESENCE_CTX_CP_THEATER = 0x14,
		PRESENCE_CTX_CP_LAST = 0x1F,
		PRESENCE_CTX_MP_FIRST = 0x20,
		PRESENCE_CTX_MP_MAIN_MENU = 0x21,
		PRESENCE_CTX_MP_CUSTOM_GAMES = 0x22,
		PRESENCE_CTX_MP_PUBLIC_MATCH = 0x23,
		PRESENCE_CTX_MP_THEATER = 0x24,
		PRESENCE_CTX_MP_LAST = 0x2F,
		PRESENCE_CTX_ZM_FIRST = 0x30,
		PRESENCE_CTX_ZM_MAIN_MENU = 0x31,
		PRESENCE_CTX_ZM_PRIVATE_GAME = 0x32,
		PRESENCE_CTX_ZM_PUBLIC_MATCH = 0x33,
		PRESENCE_CTX_ZM_THEATER = 0x34,
		PRESENCE_CTX_ZM_LAST = 0x3F,
		PRESENCE_CTX_LAST = 0x40,
	};

	enum LobbyJoinable
	{
		LOBBY_JOINABLE_NO_NOT_IN_LOBBY = 0x0,
		LOBBY_JOINABLE_YES = 0x1,
		LOBBY_JOINABLE_YES_FRIENDS_ONLY = 0x2,
		LOBBY_JOINABLE_NO_PARTY_PRIVACY_INVITE_ONLY = 0x3,
		LOBBY_JOINABLE_NO_PARTY_PRIVACY_CLOSED = 0x4,
		LOBBY_JOINABLE_NO_PARTY_FULL = 0x5,
		LOBBY_JOINABLE_NO_GAME_FULL = 0x6,
		LOBBY_JOINABLE_NO_PLAYING_LOCAL = 0x7,
		LOBBY_JOINABLE_NO_GAME_PAUSED = 0x8,
		LOBBY_JOINABLE_NO = 0x9,
		LOBBY_JOINABLE_NO_YOU_NEED_DLC = 0xA,
		LOBBY_JOINABLE_COUNT = 0xB,
	};

	enum PresencePrimary
	{
		PRESENCE_PRIMARY_UNKNOWN = 0xFFFFFFFF,
		PRESENCE_PRIMARY_OFFLINE = 0x0,
		PRESENCE_PRIMARY_ONLINE = 0x1,
		PRESENCE_PRIMARY_LOCAL = 0x2,
		PRESENCE_PRIMARY_TITLE = 0x3,
		PRESENCE_PRIMARY_AWAY = 0x4,
		PRESENCE_PRIMARY_BLOCKED = 0x5,
	};

	struct PresenceMemberData
	{
		char gamertag[17];
	};

	struct PresencePartyData
	{
		int max;
		int totalCount;
		int availableCount;
		PresenceMemberData members[18];
	};

	struct PresenceTitleData
	{
		PresenceActivity activity;
		PresenceCtx ctx;
		LobbyJoinable joinable;
		int gametypeID;
		int mapID;
		int difficulty;
		int playlist;
		int startupTimestamp;
		PresencePartyData party;
	};

	struct PresencePlatformData
	{
		PresencePrimary primaryPresence;
		char titleId[32];
		char titleName[64];
		char titleStatus[64];
	};

	struct PresenceData
	{
		int version;
		int flags;
		uint64_t xuid;
		bool isDirty;
		bool isInitialzied;
		int id;
		const char *base;
		const char *params;
		const char *data;
		int failureCount;
		int lastUpdateTime;
		int state;
		PresenceTitleData title;
		PresencePlatformData platform;
	};

	enum TaskState
	{
		TASK_STATE_INVALID = 0x0,
		TASK_STATE_INIT = 0x1,
		TASK_STATE_INPROGRESS = 0x2,
		TASK_STATE_INCALLBACK = 0x3,
		TASK_STATE_CHILDCOMPLETE = 0x4,
		TASK_STATE_CHILDFAILED = 0x5,
		TASK_STATE_COMPLETED = 0x6,
		TASK_STATE_FAILED = 0x7,
		TASK_STATE_CANCELLED = 0x8,
		TASK_STATE_COUNT = 0x9,
	};

	struct bdRemoteTask
	{
		char pad[0x38];
		uint32_t numResults;
		uint32_t maxNumResults;
		uint32_t totalNumResults;
		uint64_t transactionID;
		bdLobbyErrorCode errorCode;
		void *taskResultProcessor;
		uint8_t serviceId;
		uint8_t taskId;
	};

	struct TaskRecord
	{
		TaskRecord *next;
		const void *definition;
		TaskState state;
		ControllerIndex_t controllerIndex;
		unsigned int lastPoll;
		unsigned int lastPollMS;
		unsigned int id;
		int startMS;
		int timeOut;
		int timeIn;
		char pad[0x8];
		bdRemoteTask* remoteTask;
		TaskRecord *nestedTask;
		void* payload;
	};

	using task_callback = bool(*)(TaskRecord*);

	struct bdRichPresenceInfo
	{
		char pad[0x20];
		bool online;
		char richPresence[128];
		unsigned int count;
		char pad2[0x10];
	};

	struct dwPresenceTask
	{
		int count;
		const uint64_t *xuids;
		bdRichPresenceInfo *info;
		task_callback successCallback;
		task_callback failureCallback;
	};

	enum MapSize
	{
		MAP_SIZE_SMALL = 0x0,
		MAP_SIZE_MEDIUM = 0x1,
		MAP_SIZE_LARGE = 0x2,
		MAP_SIZE_COUNT = 0x3,
	};

	struct MapTableEntry
	{
		const char* name;
		MapSize size;
		const char* mapName;
		const char* mapNameCaps;
		const char* rootMapName;
		const char* mapDescription;
		const char* mapDescriptionShort;
		const char* missionName;
		const char* mapLocation;
		const char* mapDateTime;
		const char* presenceString;
		void *previewImage;
		const char* loadingImage;
		const char* loadingImage_axis;
		void *compassMap;
		void *compassMapCorrupt;
		bool isSafeHouse;
		bool isSubLevel;
		const char* safeHouse;
		const char* introMovie;
		const char* outroMovie;
		void* objectives;
		const char* collectibles;
		void* accoladelist;
		const char* mapObjective;
		Vec3 engagementRanges;
		const char* resistanceText;
		void *operationsBackground;
		bool isFreeRunMap;
		uint32_t freerunTrackIndex;
		uint32_t mapVersion;
		uint32_t kvpCount;
		void *kvpItems;
		int uniqueID;
	};

	struct StructuredTableHeader
	{
		const char *string;
		int hash;
		int index;
	};

	struct StructuredTableCell
	{
		int type;
		const char *string;
		int number;
		int hash;
	};

	struct StructuredTable
	{
		const char *name;
		int cellCount;
		int columnCount;
		int rowCount;
		StructuredTableCell *cells;
		int *cellIndex;
		StructuredTableHeader *headers;
		int *headerIndex;
	};

	enum StructuredTableValueType
	{
		STRUCTURED_TABLE_VALUE_TYPE_STRING = 0x0,
		STRUCTURED_TABLE_VALUE_TYPE_NUMBER = 0x1,
	};

	union StructuredTableValue
	{
		const char *strVal;
		int numVal;
	};

	struct Msg_LobbyState
	{
		int stateNum;
		int lobbyNetworkMode;
		int lobbyMainMode;
		int partyPrivacy;
		int lobbyType;
		int lobbyMode;
		int status;
		int uiScreen;
		int uiLeaderActivity;
		char key[32];
		uint64_t clientLeader;
		uint64_t platformSessionID;
		int maxClients;
		bool isAdvertised;
		int clientCount;
		char pad[0x5360];
	};

	struct Msg_LobbyStateGame
	{
		Msg_LobbyState core;
		char pad[0x1F10];
		int compressedBufferSize;
		char compressedBuffer[0x100];
	};
}
