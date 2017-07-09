#include "sdk.h"

COffsets offsets;

void Offsets::GrabOffsets()
{
	offsets.m_iHealth = Util::NetVars->GetOffset("DT_CSPlayer", "m_iHealth");
	offsets.m_iTeamNum = Util::NetVars->GetOffset("DT_CSPlayer", "m_iTeamNum");
	offsets.m_bDormant = 0xE9;
	offsets.m_bGunGameImmunity = Util::NetVars->GetOffset("DT_CSPlayer", "m_bGunGameImmunity");
	offsets.m_lifeState = Util::NetVars->GetOffset("DT_CSPlayer", "m_lifeState");
	offsets.m_fFlags = Util::NetVars->GetOffset("DT_CSPlayer", "m_fFlags");
	offsets.m_Local = Util::NetVars->GetOffset("DT_BasePlayer", "m_Local");
	offsets.m_nTickBase = Util::NetVars->GetOffset("DT_CSPlayer", "m_nTickBase");
	offsets.m_nForceBone = Util::NetVars->GetOffset("DT_CSPlayer", "m_nForceBone");
	offsets.m_mBoneMatrix = offsets.m_nForceBone + 0x1C;
	offsets.m_viewPunchAngle = Util::NetVars->GetOffset("DT_BasePlayer", "m_Local") + 0x64;
	offsets.m_aimPunchAngle = Util::NetVars->GetOffset("DT_BasePlayer", "m_Local") + 0x70;
	offsets.m_vecOrigin = Util::NetVars->GetOffset("DT_BasePlayer", "m_vecOrigin");
	offsets.m_vecViewOffset = Util::NetVars->GetOffset("DT_CSPlayer", "m_vecViewOffset[0]");
	offsets.m_vecVelocity = Util::NetVars->GetOffset("DT_CSPlayer", "m_vecVelocity[0]");
	offsets.m_szLastPlaceName = Util::NetVars->GetOffset("DT_CSPlayer", "m_szLastPlaceName");
	offsets.m_hActiveWeapon = Util::NetVars->GetOffset("DT_CSPlayer", "m_hActiveWeapon");
	offsets.m_fAccuracyPenalty = Util::NetVars->GetOffset("DT_WeaponCSBase", "m_fAccuracyPenalty");
	offsets.m_Collision = Util::NetVars->GetOffset("DT_BasePlayer", "m_Collision");
	offsets.m_iShotsFired = Util::NetVars->GetOffset("DT_CSPlayer", "m_iShotsFired");
	offsets.m_iWeaponID = Util::NetVars->GetOffset("DT_WeaponCSBase", "m_fAccuracyPenalty") + 0x2C;
	offsets.m_nMoveType = 0x258;

	offsets.m_flNextPrimaryAttack = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	offsets.m_nFallbackPaintKit = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_nFallbackPaintKit");
	offsets.m_nFallbackSeed = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_nFallbackSeed");
	offsets.m_flFallbackWear = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_flFallbackWear");
	offsets.m_nFallbackStatTrak = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_nFallbackStatTrak");
	offsets.m_AttributeManager = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_AttributeManager");
	offsets.m_Item = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_Item");
	offsets.m_iEntityLevel = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_iEntityLevel");
	offsets.m_iItemIDHigh = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_iItemIDHigh");
	offsets.m_iItemIDLow = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_iItemIDLow");
	offsets.m_iAccountID = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_iAccountID");
	offsets.m_iEntityQuality = Util::NetVars->GetOffset("DT_BaseCombatWeapon", "m_iEntityQuality");
	offsets.m_OriginalOwnerXuidLow = Util::NetVars->GetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
	offsets.m_OriginalOwnerXuidHigh = Util::NetVars->GetOffset("DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
	offsets.m_iItemDefinitionIndex = Util::NetVars->GetOffset("DT_BaseAttributableItem", "m_iItemDefinitionIndex");
}