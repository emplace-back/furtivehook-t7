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
#define VECTOR_COPY(a,b) ((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])

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

	void clear() {
		set(0.0f);
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
	
	enum team_t : __int32
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
	
	enum errorCode : __int32
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

	enum TraceBrushType : __int32
	{
		TRACE_BRUSHTYPE_NONE = 0x0,
		TRACE_BRUSHTYPE_BRUSH = 0x1,
	};
	
	enum TraceHitType : __int32
	{
		TRACE_HITTYPE_NONE = 0x0,
		TRACE_HITTYPE_ENTITY = 0x1,
		TRACE_HITTYPE_DYNENT_MODEL = 0x2,
		TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
		TRACE_HITTYPE_GLASS = 0x4,
	};
	
	enum dvarType_t : __int32
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
	
	struct msg_t
	{
		bool overflowed;
		bool readOnly;
		char *data;
		char *splitData;
		int maxsize;
		int cursize;
		int splitSize;
		int readcount;
		int bit;
		int lastEntityRef;
		bool flush;
		netsrc_t targetLocalNetID;
	};

	enum netadrtype_t
	{
		NA_BOT = 0x0,
		NA_BAD = 0x1,
		NA_LOOPBACK = 0x2,
		NA_RAWIP = 0x3,
		NA_IP = 0x4,
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

	enum MsgType
	{
		MESSAGE_TYPE_NONE = -1,
		MESSAGE_TYPE_INFO_REQUEST = 0,
		MESSAGE_TYPE_INFO_RESPONSE = 1,
		MESSAGE_TYPE_LOBBY_HOST_DISCONNECT_CLIENT = 9,
		MESSAGE_TYPE_LOBBY_HOST_LEAVE_WITH_PARTY = 10,
		MESSAGE_TYPE_LOBBY_CLIENT_HEARTBEAT = 11,
		MESSAGE_TYPE_LOBBY_CLIENT_DISCONNECT = 12,
		MESSAGE_TYPE_LOBBY_CLIENT_RELIABLE_DATA = 13,
		MESSAGE_TYPE_LOBBY_CLIENT_CONTENT = 14,
		MESSAGE_TYPE_LOBBY_MODIFIED_STATS = 15,
		MESSAGE_TYPE_JOIN_AGREEMENT_REQUEST = 18,
		MESSAGE_TYPE_PEER_TO_PEER_CONNECTIVITY_TEST = 23,
		MESSAGE_TYPE_LOBBY_MIGRATE_START = 27,
		MESSAGE_TYPE_VOICE_PACKET = 30,
		MESSAGE_TYPE_COUNT = 32, 
	};

	enum PackageType
	{
		PACKAGE_TYPE_NONE = 0x0,
		PACKAGE_TYPE_WRITE = 0x1,
		PACKAGE_TYPE_READ = 0x2,
	};
	
	struct LobbyMsg
	{
		msg_t msg;
		MsgType msgType;
		PackageType packageType;
		char encodeFlags;
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

	enum SessionActive : __int32
	{
		SESSION_INACTIVE = 0x0,
		SESSION_KEEP_ALIVE = 0x1,
		SESSION_ACTIVE = 0x2,
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

	enum connstate_t : __int32
	{
		CA_DISCONNECTED = 0x0,
		CA_CINEMATIC = 0x1,
		CA_UICINEMATIC = 0x2,
		CA_LOGO = 0x3,
		CA_CONNECTING = 0x4,
		CA_UISERVER = 0x5,
		CA_CONFIRMLOADING = 0x6,
		CA_CONNECTED = 0x7,
		CA_SENDINGDATA = 0x8,
		CA_SENDINGDATA_WAIT = 0x9,
		CA_LOADING = 0xA,
		CA_PRIMED = 0xB,
		CA_ACTIVE = 0xC,
		CA_COUNT = 0xD,
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

	struct Msg_ConnectionTest
	{
		int lobbyType;
		int clientIndex;
	};

	enum NetChanMsgType_e
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

	enum NetchanMsgMode_e
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

	struct Msg_InfoRequest
	{
		std::uint32_t nonce;
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
		char natType;
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
		char pad[0xB56C];

		// 0x11A8E0 - 0x11A8B0

		Vec3 origin() const
		{
			return *reinterpret_cast<Vec3*>(reinterpret_cast<uintptr_t>(this) + 0x30);
		}

		Vec3 view_angles() const
		{
			return *reinterpret_cast<Vec3*>(reinterpret_cast<uintptr_t>(this) + 0x318);
		}
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
		char pad[0x11A8B0];
		playerState_s predictedPlayerState;
		int lastPlayerStateOverride;
		centity_t predictedPlayerEntity;
		char pad2[0x1B21B8];
		Vec3 refdefViewAngles;
		char pad3[0xF154];
		clientInfo_t clients[18];
		char pad4[0x4A240];

		//inKillcam 0x2E77E8
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
		int leaderActivity;
		uint64_t platformSessionID;
		int lobbyTypeMoveFrom;
		int moveCount;
		uint64_t moveXuids[18];
		int team;
		int mapVote;
		bool readyUp;
		int itemVote;
		int characterSelection;
		bool editComplete;
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
		std::uint16_t partGroup;
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

	enum PenetrateType
	{
		PENETRATE_TYPE_NONE = 0x0,
		PENETRATE_TYPE_SMALL = 0x1,
		PENETRATE_TYPE_MEDIUM = 0x2,
		PENETRATE_TYPE_LARGE = 0x3,
		PENETRATE_TYPE_COUNT = 0x4,
	};

	struct WeaponDef
	{
		char pad[0x10CD];
		bool bBulletImpactExplode;
	};
}
