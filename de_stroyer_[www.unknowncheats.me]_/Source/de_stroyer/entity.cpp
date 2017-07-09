#pragma once

#include "sdk.h"

int	CBaseEntity::GetHealth()
{
	return *(int*)((DWORD)this + offsets.m_iHealth);
}

int CBaseEntity::GetTeam()
{
	return *(int*)((DWORD)this + offsets.m_iTeamNum);
}

int CBaseEntity::GetFlags()
{
	return *(int*)((DWORD)this + offsets.m_fFlags);
}

int CBaseEntity::GetTickBase()
{
	return *(int*)((DWORD)this + offsets.m_nTickBase);
}

int CBaseEntity::GetShotsFired()
{
	return *(int*)((DWORD)this + offsets.m_iShotsFired);
}

int CBaseEntity::GetMoveType()
{
	return *(int*)((DWORD)this + offsets.m_nMoveType);
}

bool CBaseEntity::GetAlive()
{
	return (bool)(*(int*)((DWORD)this + offsets.m_lifeState) == 0);
}

bool CBaseEntity::GetDormant()
{
	return *(bool*)((DWORD)this + offsets.m_bDormant);
}

bool CBaseEntity::GetImmune()
{
	return *(bool*)((DWORD)this + offsets.m_bGunGameImmunity);
}

Vector CBaseEntity::GetViewPunch()
{
	return *(Vector*)((DWORD)this + offsets.m_viewPunchAngle);
}

Vector CBaseEntity::GetPunch()
{
	return *(Vector*)((DWORD)this + offsets.m_aimPunchAngle);
}

Vector CBaseEntity::GetOrigin()
{
	return *(Vector*)((DWORD)this + offsets.m_vecOrigin);
}

Vector CBaseEntity::GetEyePosition()
{
	return *(Vector*)((DWORD)this + offsets.m_vecViewOffset) + this->GetOrigin();
}

Vector CBaseEntity::GetBonePosition(int iBone)
{
	matrix3x4_t boneMatrixes[128];
	if (this->SetupBones(boneMatrixes, 128, 0x100, 0))
	{
		matrix3x4_t boneMatrix = boneMatrixes[iBone];
		return Vector(boneMatrix.m_flMatVal[0][3], boneMatrix.m_flMatVal[1][3], boneMatrix.m_flMatVal[2][3]);
	}
	else return Vector(0, 0, 0);
}

bool CBaseEntity::SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
{
	__asm
	{
		mov edi, this
		lea ecx, dword ptr ds : [edi + 0x4]
		mov edx, dword ptr ds : [ecx]
		push currentTime
		push boneMask
		push nMaxBones
		push pBoneToWorldOut
		call dword ptr ds : [edx + 0x34]
	}
}

Vector CBaseEntity::GetVelocity()
{
	return *(Vector*)((DWORD)this + offsets.m_vecVelocity);
}

ICollideable* CBaseEntity::GetCollideable()
{
	return (ICollideable*)((DWORD)this + offsets.m_Collision);
}

player_info_t CBaseEntity::GetPlayerInfo()
{
	player_info_t pinfo;
	I::Engine->GetPlayerInfo(this->index, &pinfo);
	return pinfo;
}

std::string	CBaseEntity::GetName()
{
	return this->GetPlayerInfo().name;
}

std::string	CBaseEntity::GetSteamID()
{
	return this->GetPlayerInfo().name;
}

std::string CBaseEntity::GetLastPlace()
{
	return std::string((const char*)this + offsets.m_szLastPlaceName);
}

CBaseCombatWeapon* CBaseEntity::GetWeapon()
{
	DWORD weaponData = *(DWORD*)((DWORD)G::LocalPlayer + offsets.m_hActiveWeapon);
	int weaponEntityIndex = weaponData &= 0xFFF;
	return (CBaseCombatWeapon*)I::ClientEntList->GetClientEntity(weaponEntityIndex);
}

_ClientClass* CBaseEntity::GetClientClass()
{
	PVOID pNetworkable = (PVOID)(this + 0x8);
	typedef _ClientClass*(__thiscall* OriginalFn)(PVOID);
	return Util::GetVFunc<OriginalFn>(pNetworkable, 2)(pNetworkable);
}

int& CBaseCombatWeapon::GetWeaponID()
{
	return *(int*)((DWORD)this + offsets.m_iWeaponID);
}

float& CBaseCombatWeapon::GetNextPrimaryAttack()
{
	return *(float*)((DWORD)this + offsets.m_flNextPrimaryAttack);
}

float& CBaseCombatWeapon::GetAccuracyPenalty()
{
	return *(float*)((DWORD)this + offsets.m_fAccuracyPenalty);
}

int& CBaseCombatWeapon::GetXUIDLow()
{
	return *(int*)((DWORD)this + offsets.m_OriginalOwnerXuidLow);
}

int& CBaseCombatWeapon::GetXUIDHigh()
{
	return *(int*)((DWORD)this + offsets.m_OriginalOwnerXuidHigh);
}

int& CBaseCombatWeapon::GetEntityQuality()
{
	return *(int*)((DWORD)this + offsets.m_iEntityQuality);
}

int& CBaseCombatWeapon::GetAccountID()
{
	return *(int*)((DWORD)this + offsets.m_iAccountID);
}

int& CBaseCombatWeapon::GetItemIDHigh()
{
	return *(int*)((DWORD)this + offsets.m_iItemIDHigh);
}

int& CBaseCombatWeapon::GetItemDefinitionIndex()
{
	return *(int*)((DWORD)this + offsets.m_iItemDefinitionIndex);
}

int& CBaseCombatWeapon::GetFallbackPaintKit()
{
	return *(int*)((DWORD)this + offsets.m_nFallbackPaintKit);
}

int& CBaseCombatWeapon::GetFallbackStatTrak()
{
	return *(int*)((DWORD)this + offsets.m_nFallbackStatTrak);
}

float& CBaseCombatWeapon::GetFallbackWear()
{
	return *(float*)((DWORD)this + offsets.m_flFallbackWear);
}

WeaponInfo_t* CBaseCombatWeapon::GetCSWpnData()
{
	static DWORD GetCSWpnDataAddr = Util::FindPattern("client.dll", (PBYTE)"\x55\x8B\xEC\x81\xEC\x00\x00\x00\x00\xB8\x00\x00\x00\x00\x57", "xxxxx????x????x");
	if (GetCSWpnDataAddr)
	{
		WeaponInfo_t* retData;
		__asm
		{
			mov ecx, this
				call GetCSWpnDataAddr
				mov retData, eax
		}
		return retData;
	}
	else
	{
		return nullptr;
	}
}

_ClientClass* CBaseCombatWeapon::GetClientClass()
{
	//PVOID pNetworkable = (PVOID)(this + 0x8);
	//typedef _ClientClass*(__thiscall* OriginalFn)(PVOID);
	//return Util::GetVFunc<OriginalFn>(pNetworkable, 2)(pNetworkable);
	__asm
	{
		mov edi, this
		lea ecx, dword ptr ds : [edi + 0x8]
		mov edx, dword ptr ds : [ecx]
		call dword ptr ds : [edx + 0x8]
	}
}