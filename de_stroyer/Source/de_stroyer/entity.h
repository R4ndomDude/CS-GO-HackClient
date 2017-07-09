#pragma once

#include "sdk.h"

class _ClientClass
{
public:
	const char* GetName(void)
	{
		return *(char**)(this + 0x8);
	}
};

enum MoveType_t
{
	MOVETYPE_NONE = 0,	
	MOVETYPE_ISOMETRIC,	
	MOVETYPE_WALK,		
	MOVETYPE_STEP,		
	MOVETYPE_FLY,		
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,	
	MOVETYPE_PUSH,		
	MOVETYPE_NOCLIP,	
	MOVETYPE_LADDER,	
	MOVETYPE_OBSERVER,	
	MOVETYPE_CUSTOM,	
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4
};

class CBaseCombatWeapon;
class CBaseEntity
{
public:
	char				__pad[0x64]; 
	int					index;
	int					GetHealth();
	int					GetTeam();
	int					GetFlags();
	int					GetTickBase();
	int					GetShotsFired();
	int					GetMoveType();
	bool				GetAlive();
	bool				GetDormant();
	bool				GetImmune();
	Vector				GetViewPunch();
	Vector				GetPunch();
	Vector				GetOrigin();
	Vector				GetEyePosition();
	Vector				GetBonePosition(int iBone);
	bool				SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime);
	Vector				GetVelocity();
	ICollideable*		GetCollideable();
	player_info_t		GetPlayerInfo();
	std::string			GetName();
	std::string			GetSteamID();
	std::string			GetLastPlace();
	int&				GetXUIDLow();
	int&				GetXUIDHigh();
	CBaseCombatWeapon*	GetWeapon();
	_ClientClass*		GetClientClass();
};

class WeaponInfo_t
{
public:
	virtual void Function1() {} //
	virtual void Function2() {} //virtual void Parse(KeyValues *pKeyValuesData, const char *szWeaponName) {} //
	virtual void RefreshDynamicParameters(void) {} //
	virtual void Function4() {} //
	virtual void Function5() {} //
	virtual void Function6() {} //
	virtual void Function7() {} //
	virtual void Function8() {} //
	virtual void Function0() {} //

	char pad_0x0004[0x2]; //0x0004
	char m_pszName[32]; //0x0006 
	char pad_0x0026[0x7E]; //0x0026
	__int16 N0000002E; //0x00A4 
	char m_pszModelName[32]; //0x00A6 
	char pad_0x00C6[0x6DE]; //0x00C6
	BYTE m_IsFullAuto; //0x07A4 
	char pad_0x07A5[0x7]; //0x07A5
	__int32 m_iPrice; //0x07AC 
	float m_flArmorRatio; //0x07B0 
	char pad_0x07B4[0x10]; //0x07B4
	float m_flPenetration; //0x07C4 
	__int32 m_iDamage; //0x07C8 
	float m_flRange; //0x07CC 
	float m_flRangeModifier; //0x07D0 
	char pad_0x07D4[0x4]; //0x07D4yea
	float m_flCycleTime; //0x07D8 
};

class CBaseCombatWeapon
{
public:
	char			__pad[0x64];
	int				index;
	int&			GetWeaponID();
	float&			GetNextPrimaryAttack();
	float&			GetAccuracyPenalty();
	int&			GetXUIDLow();
	int&			GetXUIDHigh();
	int&			GetEntityQuality();
	int&			GetAccountID();
	int&			GetItemIDHigh();
	int&			GetItemDefinitionIndex();
	int&			GetFallbackPaintKit();
	int&			GetFallbackStatTrak();
	float&			GetFallbackWear();
	WeaponInfo_t*	GetCSWpnData();
	_ClientClass*	GetClientClass();
};