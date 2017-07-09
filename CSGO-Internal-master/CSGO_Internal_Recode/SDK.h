#pragma once

#pragma warning(disable : 4244)

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <future>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <sys\stat.h>

#include "VTHook.h"
#include "Vector.h"
#include "dt_recv.h"

#define PI 3.14159265358979323846f
#define RADPI 57.295779513082f
#define DEG2RAD(x)((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)((float)(x) * (float)(180.0f / (float)(PI)))
#define SAFE_RESET( x ) if( x ) { x.reset( ); x = nullptr; }
#define SAFE_DELETE( x ) if( x ) { delete x; x = nullptr; }
#define IsInRange(x, a, b) (x >= a && x <= b)
#define GetBits(x) (IsInRange(x, '0', '9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xA))
#define GetByte(x) (GetBits(x[0]) << 4 | GetBits(x[1]))

class model_t;
class CGameTrace;
class CUserCmd;
class IMatRenderContext;
struct ModelRenderInfo_t;
struct DrawModelState_t;
struct CViewSetup;
class CNetVars;
class CDrawings;
class CMisc;
class CAimbot;
class CVars;
class CPlayer;

enum ClientFrameStage_t;

typedef CGameTrace trace_t;
typedef float matrix3x4[3][4];
typedef unsigned short ModelInstanceHandle_t;
typedef unsigned long HFont;

typedef void* (*CreateInterfaceFn)(const char* szInterface, int* pReturnCode);

typedef void (__cdecl* MsgFn)(char const* pMsg, va_list);

typedef bool (__thiscall* CreateMoveFn)(void*, float, CUserCmd*);
typedef void (__thiscall* FrameStageNotifyFn)(void*, ClientFrameStage_t);
typedef void (__thiscall* PaintTraverseFn)(void*, unsigned int, bool, bool);
typedef void (__thiscall* DrawModelExecuteFn)(void*, IMatRenderContext* matctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);
typedef void (__thiscall* RenderViewFn)(void*, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);

template <typename Fn> __forceinline Fn CallVirtualFunction(void* pClassBase, int nFunctionIndex)
{
	return (Fn)((PDWORD)*(PDWORD*)pClassBase)[nFunctionIndex];
}

/*enum Vect_t
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};*/

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

enum ItemDefinitionIndex_t
{
	WEAPON_NONE,
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFEGG = 41,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_R8REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516
};

enum FontDrawType_t
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

enum FontFlags_t
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

enum PlayerControls_t
{
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_ATTACK2 = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_ALT1 = (1 << 14),
	IN_ALT2 = (1 << 15),
	IN_SCORE = (1 << 16),
	IN_SPEED = (1 << 17),
	IN_WALK = (1 << 18),
	IN_ZOOM = (1 << 19),
	IN_WEAPON1 = (1 << 20),
	IN_WEAPON2 = (1 << 21),
	IN_BULLRUSH = (1 << 22),
	IN_GRENADE1 = (1 << 23),
	IN_GRENADE2 = (1 << 24),
	IN_ATTACK3 = (1 << 25),
};

enum PlayerStates_t
{
	FL_ONGROUND = (1 << 0),
	FL_DUCKING = (1 << 1),
	FL_WATERJUMP = (1 << 2),
	FL_ONTRAIN = (1 << 3),
	FL_INRAIN = (1 << 4),
	FL_FROZEN = (1 << 5),
	FL_ATCONTROLS = (1 << 6),
	FL_CLIENT = (1 << 7),
	FL_FAKECLIENT = (1 << 8),
	FL_INWATER = (1 << 9),
};

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

enum SurfaceFlags_t
{
	DISPSURF_FLAG_SURFACE = (1 << 0),
	DISPSURF_FLAG_WALKABLE = (1 << 1),
	DISPSURF_FLAG_BUILDABLE = (1 << 2),
	DISPSURF_FLAG_SURFPROP1 = (1 << 3),
	DISPSURF_FLAG_SURFPROP2 = (1 << 4),
};

enum MoveType_t
{
	MOVETYPE_NONE = 0,			// never moves
	MOVETYPE_ISOMETRIC,			// For players -- in TF2 commander view, etc.
	MOVETYPE_WALK,				// Player only - moving on the ground
	MOVETYPE_STEP,				// gravity, special edge handling -- monsters use this
	MOVETYPE_FLY,				// No gravity, but still collides with stuff
	MOVETYPE_FLYGRAVITY,		// flies through the air + is affected by gravity
	MOVETYPE_VPHYSICS,			// uses VPHYSICS for simulation
	MOVETYPE_PUSH,				// no clip to world, push and crush
	MOVETYPE_NOCLIP,			// No gravity, no collisions, still do velocity/avelocity
	MOVETYPE_LADDER,			// Used by players only when going onto a ladder
	MOVETYPE_OBSERVER,			// Observer movement, depends on player's observer mode
	MOVETYPE_CUSTOM,			// Allows the entity to describe its own physics

	// should always be defined as the last item in the list
	MOVETYPE_LAST = MOVETYPE_CUSTOM,

	MOVETYPE_MAX_BITS = 4,
};

enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),

	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_MULTIPASS = (1 << 9),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17),
	MATERIAL_VAR_NOALPHAMOD = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19),
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23),
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25),
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};

enum LifeStates_t
{
	LIFE_ALIVE = 0, // alive
	LIFE_DYING,		// playing death animation or still falling off of a ledge waiting to hit ground
	LIFE_DEAD,		// dead. lying still.
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY,
};

enum PlayerHitboxes_t
{
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_LOWER_NECK,
	HITBOX_PELVIS,
	HITBOX_BODY,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX,
};

enum Sequence_Default
{
	SEQUENCE_DEFAULT_DRAW = 0,
	SEQUENCE_DEFAULT_IDLE1 = 1,
	SEQUENCE_DEFAULT_IDLE2 = 2,
	SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
	SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
	SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
	SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
	SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
	SEQUENCE_DEFAULT_LOOKAT01 = 12,
};

enum ConsolePrint_t
{
	PRINT_MSG = 0,
	PRINT_WARNING,
	PRINT_ERROR,
};

struct mstudiobbox_t
{
	int					bone;
	int					group;
	Vector3				bbmin;
	Vector3				bbmax;
	int					szhitboxnameindex;
	int					m_iPad01[3];
	float				m_flRadius;
	int					m_iPad02[4];
};

struct mstudiohitboxset_t
{
	int					sznameindex;
	inline char * const	pszName(void) const { return ((char *)this) + sznameindex; }
	int					numhitboxes;
	int					hitboxindex;
	inline mstudiobbox_t *pHitbox(int i) const { return (mstudiobbox_t*)(((unsigned char*)this) + hitboxindex) + i; };
};

struct studiohdr_t
{
	unsigned char		pad00[12];
	char				name[64];
	unsigned char		pad01[80];
	int					numbones;
	int					boneindex;
	unsigned char		pad02[12];
	int					hitboxsetindex;
	unsigned char		pad03[228];

	mstudiohitboxset_t* pHitboxSet(int i) const
	{
		return (mstudiohitboxset_t*)(((unsigned char *)this) + hitboxsetindex) + i;
	};

	mstudiobbox_t* pHitbox(int i, int set) const
	{
		const mstudiohitboxset_t* s = pHitboxSet(set);
		if(!s)
			return NULL;

		return s->pHitbox(i);
	};

	int			iHitboxCount(int set) const
	{
		const mstudiohitboxset_t* s = pHitboxSet(set);
		if(!s)
			return 0;

		return s->numhitboxes;
	};
};

struct ModelRenderInfo_t
{
	Vector3 origin;
	Vector3 angles;
	void* pRenderable;
	const model_t* pModel;
	const matrix3x4* pModelToWorld;
	const matrix3x4* pLightingOffset;
	const Vector3* pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	ModelInstanceHandle_t instance;

	ModelRenderInfo_t()
	{
		pModelToWorld = nullptr;
		pLightingOffset = nullptr;
		pLightingOrigin = nullptr;
	}
};

typedef struct player_info_s
{
	DWORD __pad0[2];
	int xuidlow;
	int xuidhigh;
	char name[128];//32
	int userid;
	char steamid[33];//32
	UINT steam3id;
	char friendsname[128];
	bool isfakeplayer;
	bool ishltv;
	DWORD customfiles[4];
	BYTE filesdownloaded;
	int __pad1;
} player_info_t;

class CUserCmd
{
public:
	virtual ~CUserCmd() { };

	int		command_number;		// 0x04 For matching server and client commands for debugging
	int		tick_count;			// 0x08 the tick the client created this command
	Vector3	viewangles;			// 0x0C Player instantaneous view angles.
	Vector3	aimdirection;		// 0x18
	float	forwardmove;		// 0x24
	float	sidemove;			// 0x28
	float	upmove;				// 0x2C
	int		buttons;			// 0x30 Attack button states
	byte    impulse;			// 0x34
	int		weaponselect;		// 0x38 Current weapon id
	int		weaponsubtype;		// 0x3C
	int		random_seed;		// 0x40 For shared random functions
	short	mousedx;			// 0x44 mouse accum in x from create move
	short	mousedy;			// 0x46 mouse accum in y from create move
	bool	hasbeenpredicted;	// 0x48 Client only, tracks whether we've predicted this command at least once
	char	pad_0x4C[0x18];		// 0x4C Current sizeof( usercmd ) =  100  = 0x64
};

struct CViewSetup
{
	char _0x0000[16];
	__int32 x;
	__int32 x_old;
	__int32 y;
	__int32 y_old;
	__int32 width;
	__int32    width_old;
	__int32 height;
	__int32    height_old;
	char _0x0030[128];
	float fov;
	float fovViewmodel;
	Vector3 origin;
	Vector3 angles;
	float zNear;
	float zFar;
	float zNearViewmodel;
	float zFarViewmodel;
	float m_flAspectRatio;
	float m_flNearBlurDepth;
	float m_flNearFocusDepth;
	float m_flFarFocusDepth;
	float m_flFarBlurDepth;
	float m_flNearBlurRadius;
	float m_flFarBlurRadius;
	float m_nDoFQuality;
	__int32 m_nMotionBlurMode;
	char _0x0104[68];
	__int32 m_EdgeBlur;
};

class CBaseHudChat
{
public:
	void ChatPrintf(int iPlayerIndex, int iFilter, const char* fmt, ...)
	{
		typedef void(__cdecl* ChatPrintfFn)(void*, int, int, const char*, ...);
		return CallVirtualFunction<ChatPrintfFn>(this, 26)(this, iPlayerIndex, iFilter, fmt);
	}

	void ChatPrintfW(int iPlayerIndex, int iFilter, const wchar_t* fmt, ...)
	{
		typedef void(__cdecl* ChatPrintfWFn)(void*, int, int, const wchar_t*, ...);
		return CallVirtualFunction<ChatPrintfWFn>(this, 27)(this, iPlayerIndex, iFilter, fmt);
	}
};

class ClientClass
{
public:
	BYTE _chPadding[8];
	char* szName;
	RecvTable* rtTable;
	ClientClass* pNextClass;
	int iClassID;
};

class IBaseClientDLL
{
public:
	ClientClass* GetAllClasses()
	{
		typedef ClientClass* (__thiscall* GetAllClassesFn)(void*);
		return CallVirtualFunction<GetAllClassesFn>(this, 8)(this);
	}
};

class IClientMode
{
public:
	char _pad[28];

	CBaseHudChat*	m_pChatElement; //0x2A
};

class IVEngineClient
{
public:
	void GetScreenSize(int& width, int& height)
	{
		typedef void(__thiscall* GetScreenSizeFn)(void*, int&, int&);
		return CallVirtualFunction<GetScreenSizeFn>(this, 5)(this, width, height);
	}

	void ClientCmd(const char* szCommandString)
	{
		typedef void(__thiscall* ClientCmdFn)(void*, const char*);
		return CallVirtualFunction<ClientCmdFn>(this, 7)(this, szCommandString);
	}

	bool GetPlayerInfo(int index, player_info_t* pInfo)
	{
		typedef bool(__thiscall* GetPlayerInfoFn)(void*, int, player_info_t*);
		return CallVirtualFunction<GetPlayerInfoFn>(this, 8)(this, index, pInfo);
	}

	int GetLocalPlayer()
	{
		typedef int(__thiscall* GetLocalPlayerFn)(void*);
		return CallVirtualFunction<GetLocalPlayerFn>(this, 12)(this);
	}

	float Time()
	{
		typedef float(__thiscall* TimeFn)(void*);
		return CallVirtualFunction<TimeFn>(this, 14)(this);
	}

	void GetViewAngles(Vector3 &ang)
	{
		typedef void(__thiscall* GetViewAnglesFn)(void*, Vector3&);
		return CallVirtualFunction<GetViewAnglesFn>(this, 18)(this, ang);
	}

	void SetViewAngles(Vector3 &ang)
	{
		typedef void(__thiscall* SetViewAnglesFn)(void*, Vector3&);
		return CallVirtualFunction<SetViewAnglesFn>(this, 19)(this, ang);
	}

	int GetMaxClients()
	{
		typedef int(__thiscall* GetMaxClientsFn)(void*);
		return CallVirtualFunction<GetMaxClientsFn>(this, 20)(this);
	}

	bool IsInGame()
	{
		typedef bool(__thiscall* IsInGameFn)(void*);
		return CallVirtualFunction<IsInGameFn>(this, 26)(this);
	}

	bool IsConnected()
	{
		typedef bool(__thiscall* IsConnectedFn)(void*);
		return CallVirtualFunction<IsConnectedFn>(this, 27)(this);
	}

	const matrix3x4& WorldToScreenMatrix()
	{
		typedef const matrix3x4& (__thiscall* GetMatrixFn)(void*);
		return CallVirtualFunction<GetMatrixFn>(this, 37)(this);
	}
};

class ICollideable
{
public:
	const Vector3& OBBMins()
	{
		typedef const Vector3& (__thiscall* OBBMinsFn)(void*);
		return CallVirtualFunction<OBBMinsFn>(this, 1)(this);
	}

	const Vector3& OBBMaxs()
	{
		typedef const Vector3& (__thiscall* OBBMaxsFn)(void*);
		return CallVirtualFunction<OBBMaxsFn>(this, 2)(this);
	}

	const Vector3& GetCollisionOrigin()
	{
		typedef const Vector3& (__thiscall* GetCollisionOriginFn)(void*);
		return CallVirtualFunction<GetCollisionOriginFn>(this, 8)(this);
	}

	const Vector3& GetCollisionAngles()
	{
		typedef const Vector3& (__thiscall* GetCollisionAnglesFn)(void*);
		return CallVirtualFunction<GetCollisionAnglesFn>(this, 9)(this);
	}

	const matrix3x4& CollisionToWorldTransform()
	{
		typedef const matrix3x4& (__thiscall* CollisionToWorldTransformFn)(void*);
		return CallVirtualFunction<CollisionToWorldTransformFn>(this, 10)(this);
	}

	int GetCollisionGroup()
	{
		typedef int(__thiscall* GetCollisionGroupFn)(void*);
		return CallVirtualFunction<GetCollisionGroupFn>(this, 14)(this);
	}
};

class IClientEntity
{
public:
	void* GetClientRenderable()
	{
		return reinterpret_cast<void*>(this + 0x4);
	}

	void* GetClientNetworkable()
	{
		return reinterpret_cast<void*>(this + 0x8);
	}

	ICollideable* GetCollideable()
	{
		typedef ICollideable* (__thiscall* GetCollideableFn)(void*);
		return CallVirtualFunction<GetCollideableFn>(this, 2)(this);
	}

	Vector3& GetAbsOrigin()
	{
		typedef Vector3& (__thiscall* GetAbsOriginFn)(void*);
		return CallVirtualFunction<GetAbsOriginFn>(this, 10)(this);
	}

	Vector3& GetAbsAngles()
	{
		typedef Vector3& (__thiscall* GetAbsAnglesFn)(void*);
		return CallVirtualFunction<GetAbsAnglesFn>(this, 11)(this);
	}

	ClientClass* GetClientClass()
	{
		typedef ClientClass* (__thiscall* GetClientClassFn)(void*);
		return CallVirtualFunction<GetClientClassFn>(this->GetClientNetworkable(), 2)(this->GetClientNetworkable());
	}

	bool IsDormant()
	{
		typedef bool(__thiscall* IsDormantFn)(void*);
		return CallVirtualFunction<IsDormantFn>(this->GetClientNetworkable(), 9)(this->GetClientNetworkable());
	}

	int GetIndex()
	{
		typedef int(__thiscall* GetIndexFn)(void*);
		return CallVirtualFunction<GetIndexFn>(this->GetClientNetworkable(), 10)(this->GetClientNetworkable());
	}

	model_t* GetModel()
	{
		typedef model_t* (__thiscall* GetModelFn)(void*);
		return CallVirtualFunction<GetModelFn>(this->GetClientRenderable(), 8)(this->GetClientRenderable());
	}

	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		typedef bool(__thiscall* SetupBonesFn)(void*, matrix3x4*, int, int, float);
		return CallVirtualFunction<SetupBonesFn>(this->GetClientRenderable(), 13)(this->GetClientRenderable(), pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}

	template <typename t>
	inline t* cast_to()
	{
		return reinterpret_cast<t*>(this);
	}
};

class IClientEntityList
{
public:
	template <typename t>
	t* GetClientEntity(int index)
	{
		typedef t* (__thiscall* GetClientEntityFn)(void*, int);
		return CallVirtualFunction<GetClientEntityFn>(this, 3)(this, index);
	}

	IClientEntity* GetClientEntityFromHandle(DWORD handle)
	{
		typedef IClientEntity* (__thiscall* GetClientEntityFromHandleFn)(void*, int); //DWORD instead of int?
		return CallVirtualFunction<GetClientEntityFromHandleFn>(this, 4)(this, handle);
	}

	int GetHighestEntityIndex()
	{
		typedef int(__thiscall* GetHighestEntityIndexFn)(void*);
		return CallVirtualFunction<GetHighestEntityIndexFn>(this, 6)(this);
	}
};

class Color
{
public:
	Color() : R(0), G(0), B(0), A(0)
	{
	}

	Color(int r, int g, int b, int a) : R(r), G(g), B(b), A(a)
	{
	}

	Color(int r, int g, int b) : R(r), G(g), B(b), A(255)
	{
	}

	static Color White(){ return Color(255, 255, 255, 255); }
	static Color Black() { return Color(0, 0, 0, 255); }
	static Color Red() { return Color(255, 0, 0, 255); }
	static Color Green() { return Color(0, 255, 0, 255); }
	static Color Blue() { return Color(0, 0, 255, 255); }
	static Color Yellow() { return Color(255, 255, 0, 255); }

	int r() { return R; }
	int g() { return G; }
	int b() { return B; }
	int a() { return A; }

	Color& operator = (Color& c)
	{
		R = c.r();
		G = c.g();
		B = c.b();
		A = c.a();
		return *this;
	}

private:
	int R, G, B, A;
};

class fColor
{
public:
	fColor() : R(0.0f), G(0.0f), B(0.0f), A(0.0f)
	{
	}

	fColor(float r, float g, float b, float a) : R(r), G(g), B(b), A(a)
	{
	}

	fColor(float r, float g, float b) : R(r), G(g), B(b), A(1.0f)
	{
	}

	static fColor White(){ return fColor(1.0f, 1.0f, 1.0f, 1.0f); }
	static fColor Black() { return fColor(0.0f, 0.0f, 0.0f, 1.0f); }
	static fColor Red() { return fColor(1.0f, 0.0f, 0.0f, 1.0f); }
	static fColor Green() { return fColor(0.0f, 1.0f, 0.0f, 1.0f); }
	static fColor Blue() { return fColor(0.0f, 0.0f, 1.0f, 1.0f); }
	static fColor Yellow() { return fColor(1.0f, 1.0f, 0.0f, 1.0f); }
	static fColor Chams_Red() { return fColor(0.8f, 0.0f, 0.0f, 1.0f); }
	static fColor Chams_Yellow() { return fColor(0.8f, 0.8f, 0.0f, 1.0f); }
	static fColor Chams_Blue() { return fColor(0.0f, 0.3f, 0.8f, 1.0f); }
	static fColor Chams_Green() { return fColor(0.5f, 0.8f, 0.0f, 1.0f); }

	float r() { return R; }
	float g() { return G; }
	float b() { return B; }
	float a() { return A; }

	fColor& operator = (fColor& c)
	{
		R = c.r();
		G = c.g();
		B = c.b();
		A = c.a();
		return *this;
	}

private:
	float R, G, B, A;
};

class IPanel
{
public:
	const char* GetName(int panel)
	{
		typedef const char* (__thiscall* GetNameFn)(void*, int);
		return CallVirtualFunction<GetNameFn>(this, 36)(this, panel);
	}
};

class ISurface
{
public:
	void DrawSetColor(Color clr)
	{
		typedef void(__thiscall* DrawSetColorFn)(void*, Color);
		return CallVirtualFunction<DrawSetColorFn>(this, 15)(this, clr);
	}

	void DrawFilledRect(int x, int y, int w, int h)
	{
		typedef void(__thiscall* DrawFilledRectFn)(void*, int, int, int, int);
		return CallVirtualFunction<DrawFilledRectFn>(this, 16)(this, x, y, w, h);
	}

	void DrawOutlinedRect(int x, int y, int w, int h)
	{
		typedef void(__thiscall* DrawOutlinedRectFn)(void*, int, int, int, int);
		return CallVirtualFunction<DrawOutlinedRectFn>(this, 18)(this, x, y, w, h);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* DrawOutlinedRectFn)(void*, int, int, int, int);
		return CallVirtualFunction<DrawOutlinedRectFn>(this, 19)(this, x0, y0, x1, y1);
	}

	void DrawSetTextFont(unsigned long index)
	{
		typedef void(__thiscall* DrawSetTextFontFn)(void*, unsigned long);
		return CallVirtualFunction<DrawSetTextFontFn>(this, 23)(this, index);
	}

	void DrawSetTextColor(Color clr)
	{
		typedef void(__thiscall* DrawSetTextColorFn)(void*, Color);
		return CallVirtualFunction<DrawSetTextColorFn>(this, 25)(this, clr);
	}

	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* DrawSetTextPosFn)(void*, int, int);
		return CallVirtualFunction<DrawSetTextPosFn>(this, 26)(this, x, y);
	}

	void DrawPrintText(wchar_t* text, int length, FontDrawType_t drawtype = FontDrawType_t::FONT_DRAW_DEFAULT)
	{
		typedef void(__thiscall* DrawPrintTextFn)(void*, wchar_t*, int, FontDrawType_t);
		return CallVirtualFunction<DrawPrintTextFn>(this, 28)(this, text, length, drawtype);
	}

	HFont CreateFnt()
	{
		typedef HFont(__thiscall* CreateFntFn)(void*);
		return CallVirtualFunction<CreateFntFn>(this, 71)(this);
	}

	bool SetFontGlyphSet(unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0)
	{
		typedef bool(__thiscall* SetFontGlyphSetFn)(void*, unsigned long, const char*, int, int, int, int, int, int, int);
		return CallVirtualFunction<SetFontGlyphSetFn>(this, 72)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax);
	}

	bool AddCustomFontFile(const char* fontFileName)
	{
		typedef bool(__thiscall* AddCustomFontFileFn)(void*, const char*);
		return CallVirtualFunction<AddCustomFontFileFn>(this, 73)(this, fontFileName);
	}

	bool GetTextSize(unsigned long font, const wchar_t* text, int& wide, int& tall)
	{
		typedef bool(__thiscall* GetTextSizeFn)(void*, unsigned long, const wchar_t*, int&, int&);
		return CallVirtualFunction<GetTextSizeFn>(this, 79)(this, font, text, wide, tall);
	}

	void PlaySnd(const char* fileName)
	{
		typedef void(__thiscall* PlaySndFn)(void*, const char*);
		return CallVirtualFunction<PlaySndFn>(this, 82 /*81*/)(this, fileName);
	}
};

struct csurface_t
{
	const char		*name;
	short			surfaceProps;
	unsigned short	flags;
};

struct cplane_t
{
	Vector3	normal;
	float	dist;
	BYTE	type;
	BYTE	signbits;
	BYTE	pad[2];
};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(void *pEntity, int contentsMask) = 0;
	virtual TraceType_t	GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter
{
public:
	bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

class CTraceFilterSkipTwoEntities : public ITraceFilter
{
public:
	CTraceFilterSkipTwoEntities(void* pPassEnt1, void* pPassEnt2)
	{
		passentity1 = pPassEnt1;
		passentity2 = pPassEnt2;
	}

	virtual bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == passentity1 || pEntityHandle == passentity2);
	}

	virtual TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* passentity1;
	void* passentity2;
};

class __declspec(align(16))VectorAligned : public Vector3
{
public:
	VectorAligned& operator=(const Vector3 &vOther)
	{
		Set(vOther.x, vOther.y, vOther.z);
		return *this;
	}
	float w;
};

struct Ray_t
{
	VectorAligned   m_Start;
	VectorAligned   m_Delta;
	VectorAligned   m_StartOffset;
	VectorAligned   m_Extents;

	const   matrix3x4* m_pWorldAxisTransform;

	bool    m_IsRay;
	bool    m_IsSwept;

	Ray_t() : m_pWorldAxisTransform(0) { }

	void Init(Vector3& start, Vector3& end)
	{
		m_Delta = end - start;

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_Extents.Set();
		m_pWorldAxisTransform = 0;
		m_IsRay = true;

		m_StartOffset.Set();
		m_Start = start;
	}

	void Init(Vector3& start, Vector3& end, Vector3& mins, Vector3& maxs)
	{
		m_Delta = end - start;

		m_pWorldAxisTransform = 0;
		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_Extents = maxs - mins;
		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.LengthSqr() < 1e-6);

		m_StartOffset = mins + maxs;
		m_StartOffset *= 0.5f;
		m_Start = start - m_StartOffset;
		m_StartOffset *= -1.0f;
	}
};

class CBaseTrace
{
public:
	bool IsDispSurface(void)				{ return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0); }
	bool IsDispSurfaceWalkable(void)		{ return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
	bool IsDispSurfaceBuildable(void)		{ return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
	bool IsDispSurfaceProp1(void)			{ return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
	bool IsDispSurfaceProp2(void)			{ return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

public:
	Vector3			startpos;
	Vector3			endpos;
	cplane_t		plane;

	float			fraction;

	int				contents;
	unsigned short	dispFlags;

	bool			allsolid;
	bool			startsolid;

	CBaseTrace() {}

private:
	CBaseTrace(const CBaseTrace& vOther);
};

class CGameTrace : public CBaseTrace
{
public:
	bool DidHitWorld() const;

	bool DidHitNonWorldEntity() const;

	int GetEntityIndex() const;

	bool DidHit() const
	{
		return fraction < 1 || allsolid || startsolid;
	}

public:
	float			fractionleftsolid;
	csurface_t		surface;

	int				hitgroup;

	short			physicsbone;
	unsigned short	worldSurfaceIndex;

	IClientEntity*	m_pEnt;
	int				hitbox;

	CGameTrace() {}
	CGameTrace(const CGameTrace& vOther);
};

class IEngineTrace
{
public:
	void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace)
	{
		typedef void(__thiscall* TraceRayFn)(void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t*);
		return CallVirtualFunction<TraceRayFn>(this, 5)(this, ray, fMask, pTraceFilter, pTrace);
	}
};

class IVModelInfoClient
{
public:
	model_t* GetModel(int index)
	{
		typedef model_t*(__thiscall* GetModelFn)(void*, int);
		return CallVirtualFunction<GetModelFn>(this, 1)(this, index);
	}

	int	GetModelIndex(const char* name)
	{
		typedef int(__thiscall* GetModelIndexFn)(void*, const char*);
		return CallVirtualFunction<GetModelIndexFn>(this, 2)(this, name);
	}

	const char* GetModelName(const model_t* model)
	{
		typedef const char* (__thiscall* GetModelNameFn)(void*, const model_t*);
		return CallVirtualFunction<GetModelNameFn>(this, 3)(this, model);
	}

	studiohdr_t* GetStudiomodel(const model_t* mod)
	{
		typedef studiohdr_t* (__thiscall* GetStudiomodelFn)(void*, const model_t*);
		return CallVirtualFunction<GetStudiomodelFn>(this, 30)(this, mod);
	}
};

class IVRenderView
{
public:
	void SetBlend(float blend)
	{
		typedef void(__thiscall* SetBlendFn)(void*, float);
		CallVirtualFunction<SetBlendFn>(this, 4)(this, blend);
	}

	void SetColorModulation(float const* blend)
	{
		typedef void(__thiscall* SetColorModulationFn)(void*, float const*);
		CallVirtualFunction<SetColorModulationFn>(this, 6)(this, blend);
	}
};

class IMaterial
{
public:
	const char* GetName()
	{
		typedef const char* (__thiscall* GetNameFn)(void*);
		return CallVirtualFunction<GetNameFn>(this, 0)(this);
	}

	const char* GetTextureGroupName()
	{
		typedef const char* (__thiscall* GetTextureGroupNameFn)(void*);
		return CallVirtualFunction<GetTextureGroupNameFn>(this, 1)(this);
	}

	void IncrementReferenceCount()
	{
		typedef void(__thiscall* IncrementReferenceCountFn)(void*);
		CallVirtualFunction<IncrementReferenceCountFn>(this, 14)(this);
	}

	void AlphaModulate(float alpha)
	{
		typedef void(__thiscall* AlphaModulateFn)(void*, float);
		CallVirtualFunction<AlphaModulateFn>(this, 27)(this, alpha);
	}

	void ColorModulate(float r, float g, float b)
	{
		typedef void(__thiscall* ColorModulateFn)(void*, float, float, float);
		CallVirtualFunction<ColorModulateFn>(this, 28)(this, r, g, b);
	}

	void SetMaterialVarFlag(MaterialVarFlags_t flag, bool on)
	{
		typedef void(__thiscall* SetMaterialVarFlagFn)(void*, MaterialVarFlags_t, bool);
		CallVirtualFunction<SetMaterialVarFlagFn>(this, 29)(this, flag, on);
	}
};

class IMaterialSystem
{
public:
	IMaterial* FindMaterial(const char* pMaterialName, const char* pTextureGroupName = "Model textures", bool complain = true, const char* pComplainPrefix = NULL)
	{
		typedef IMaterial* (__thiscall* FindMaterialFn)(void*, const char*, const char*, bool, const char*);
		return CallVirtualFunction<FindMaterialFn>(this, 84)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
	}
};

class IVModelRender
{
public:
	void ForcedMaterialOverride(IMaterial* newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL, int unk = 0)
	{
		typedef void(__thiscall* ForcedMaterialOverrideFn)(void*, IMaterial*, OverrideType_t, int);
		CallVirtualFunction<ForcedMaterialOverrideFn>(this, 1)(this, newMaterial, nOverrideType, unk);
	}

	void DrawModelExecute(IMatRenderContext* matctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4* pCustomBoneToWorld)
	{
		typedef void(__thiscall* DrawModelExecuteFn)(void*, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4*);
		CallVirtualFunction<DrawModelExecuteFn>(this, 21)(this, matctx, state, pInfo, pCustomBoneToWorld);
	}
};

class IGameMovement
{
public:
	void ProcessMovement(IClientEntity* ent, void* data)
	{
		typedef void(__thiscall* ProcessMovementFn)(void*, IClientEntity*, void*);
		CallVirtualFunction<ProcessMovementFn>(this, 1)(this, ent, data);
	}

	void DecayPunchangle()
	{
		typedef void(__thiscall* DecayPunchangleFn)(void*);
		return CallVirtualFunction<DecayPunchangleFn>(this, 76)(this);
	}
};

class CGlobalVarsBase
{
public:
	float	realtime;
	int		framecount;
	float	absoluteframetime;
	float	absoluteframestarttimestddev;
	float	curtime;
	float	frametime;
	int		maxclients;
	int		tickcount;
	float	interval_per_tick;
	float	interpolation_amount;
	int		simTicksThisFrame;
	int		network_protocol;
	char	pad00[14];
};

class IPlayerInfoManager
{
public:
	CGlobalVarsBase* GetGlobalVars()
	{
		typedef CGlobalVarsBase*(__thiscall* GetGlobalVarsFn)(void*);
		return CallVirtualFunction<GetGlobalVarsFn>(this, 1)(this);
	}
};

class IViewRender
{
public:
};

class Hack_
{
public:
	std::shared_ptr<CNetVars> NetVars = nullptr;
	std::shared_ptr<CDrawings> Drawings = nullptr;
	std::shared_ptr<CMisc> Misc = nullptr;
	std::shared_ptr<CAimbot> Aimbot = nullptr;
	std::shared_ptr<CVars> CVars = nullptr;

public:
	std::shared_ptr<CHook> ClientModeHook = nullptr;
	std::shared_ptr<CHook> PanelHook = nullptr;
	std::shared_ptr<CHook> ModelRenderHook = nullptr;
	std::shared_ptr<CHook> ViewRenderHook = nullptr;

	RecvVarProxyFn oModelIndex = nullptr;
	RecvVarProxyFn oItemDefinitionIndex = nullptr;

public:
	IMaterial* ChamsMat = nullptr;
	IMaterial* ChamsMat_IgnoreZ = nullptr;
	IMaterial* ChamsMat_Flat = nullptr;
	IMaterial* ChamsMat_Flat_IgnoreZ = nullptr;
};

class Interfaces_
{
public:
	IBaseClientDLL* Client = nullptr;
	IClientMode* ClientMode = nullptr;
	IClientEntityList* EntList = nullptr;
	IVEngineClient* Engine = nullptr;
	IPanel* Panel = nullptr;
	ISurface* Surface = nullptr;
	CGlobalVarsBase* Globals = nullptr;
	IGameMovement* GameMovement = nullptr;
	IPlayerInfoManager* PlayerInfoManager = nullptr;
	IEngineTrace* EngineTrace = nullptr;
	IVModelInfoClient* ModelInfo = nullptr;
	IVModelRender* ModelRender = nullptr;
	//IVRenderView* RenderView = nullptr;
	IMaterialSystem* MatSystem = nullptr;
	IViewRender* ViewRender = nullptr;
};

class NetVars_
{
public:
	DWORD m_rgflCoordinateFrame = 0;
	DWORD m_Collision = 0;
	DWORD m_vecOrigin = 0;
	DWORD m_vecViewOffset = 0;
	DWORD m_vecPunchAngles = 0;
	DWORD m_vecViewPunchAngles = 0;
	DWORD m_vecVelocity = 0;
	DWORD m_bIsScoped = 0;
	DWORD m_lifeState = 0;
	DWORD m_fFlags = 0;
	DWORD m_iHealth = 0;
	DWORD m_iTeamNum = 0;
	DWORD m_iGlowIndex = 0;
	DWORD m_iShotsFired = 0;
	DWORD m_hActiveWeapon = 0;
	DWORD m_hMyWeapons = 0;
	DWORD m_iItemDefinitionIndex = 0;
	DWORD m_nHitboxSet = 0;
	DWORD m_nModelIndex = 0;
	DWORD m_hOwner = 0;
	DWORD m_hWeapon = 0;
	DWORD m_hViewModel = 0;
	DWORD m_nFallbackPaintKit = 0;
	DWORD m_iItemIDHigh = 0;
};

class ScreenSize_
{
public:
	int Width = 0;
	int Height = 0;
};

extern Hack_ Hack;
extern Interfaces_ Interfaces;
extern NetVars_ NetVars;
extern ScreenSize_ ScreenSize;