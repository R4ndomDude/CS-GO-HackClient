#include "Player.h"
#include "NetVars.h"
#include "Misc.h"

bool CPlayer::IsValid()
{
	if(!this)
		return false;

	if(this->GetHealth() < 1)
		return false;

	if(this->GetLifeState() != LIFE_ALIVE)
		return false;

	if(this->IsDormant())
		return false;

	return true;
}

bool CPlayer::IsValid(CPlayer* pPlayer)
{
	if(!this->IsValid())
		return false;

	if(this->GetIndex() == pPlayer->GetIndex())
		return false;

	if(this->GetTeam() == pPlayer->GetTeam())
		return false;

	return true;
}

Vector3 CPlayer::GetOrigin()
{
	return *reinterpret_cast<Vector3*>(this + NetVars.m_vecOrigin);
}

Vector3 CPlayer::GetEyePosition()
{
	return *reinterpret_cast<Vector3*>(this + NetVars.m_vecViewOffset) + GetOrigin();
}

Vector3 CPlayer::GetPunchAngles()
{
	return *reinterpret_cast<Vector3*>(this + NetVars.m_vecPunchAngles);
}

Vector3 CPlayer::GetViewPunchAngles()
{
	return *reinterpret_cast<Vector3*>(this + NetVars.m_vecViewPunchAngles);
}

Vector3 CPlayer::GetVelocity()
{
	return *reinterpret_cast<Vector3*>(this + NetVars.m_vecVelocity);
}

Vector3 CPlayer::GetBonePosition(int bone)
{
	matrix3x4 matrix[128];

	this->SetupBones(matrix, 128, 0x100, Interfaces.Globals->curtime);

	return Vector3(matrix[bone][0][3], matrix[bone][1][3], matrix[bone][2][3]);
}

Vector3 CPlayer::GetHitboxPosition(int hitbox)
{
	model_t* model = this->GetModel();
	if(!model)
		return Vector3(0.0f, 0.0f, 0.0f);

	studiohdr_t* hdr = Interfaces.ModelInfo->GetStudiomodel(model);
	if(!hdr)
		return Vector3(0.0f, 0.0f, 0.0f);

	matrix3x4 matrix[128];
	if(!this->SetupBones(matrix, 128, 0x100, Interfaces.Globals->curtime))
		return Vector3(0.0f, 0.0f, 0.0f);

	mstudiohitboxset_t *set = hdr->pHitboxSet(GetHitboxSet());
	if(!set)
		return Vector3(0.0f, 0.0f, 0.0f);

	mstudiobbox_t* box = set->pHitbox(hitbox);
	if(!box)
		return Vector3(0.0f, 0.0f, 0.0f);

	Vector3 center = ((box->bbmin + box->bbmax) * .5f);

	Vector3 hitboxpos;

	Hack.Misc->VectorTransform(center, matrix[box->bone], hitboxpos);

	return hitboxpos;
}

bool CPlayer::IsScoped()
{
	return *reinterpret_cast<bool*>(this + NetVars.m_bIsScoped);
}

bool CPlayer::GetDormant()
{
	return *reinterpret_cast<bool*>(this + 0xE9);
}

MoveType_t CPlayer::GetMoveType()
{
	return *reinterpret_cast<MoveType_t*>(this + 0x258);
}

byte CPlayer::GetLifeState()
{
	return *reinterpret_cast<byte*>(this + NetVars.m_lifeState);
}

int CPlayer::GetFlags()
{
	return *reinterpret_cast<int*>(this + NetVars.m_fFlags);
}

int CPlayer::GetHealth()
{
	return *reinterpret_cast<int*>(this + NetVars.m_iHealth);
}

int CPlayer::GetTeam()
{
	return *reinterpret_cast<int*>(this + NetVars.m_iTeamNum);
}

int CPlayer::GetGlowIndex()
{
	return *reinterpret_cast<int*>(this + NetVars.m_iGlowIndex);
}

int CPlayer::GetShotsFired()
{
	return *reinterpret_cast<int*>(this + NetVars.m_iShotsFired);
}

int CPlayer::GetHitboxSet()
{
	return *reinterpret_cast<int*>(this + NetVars.m_nHitboxSet);
}

int CPlayer::GetUserId()
{
	player_info_t player_info;

	Interfaces.Engine->GetPlayerInfo(this->GetIndex(), &player_info);

	return player_info.userid;
}

UINT* CPlayer::GetActiveWeapon()
{
	return reinterpret_cast<UINT*>(this + NetVars.m_hActiveWeapon);
}

UINT* CPlayer::GetWeapons()
{
	return reinterpret_cast<UINT*>(this + NetVars.m_hMyWeapons); // NetVars.m_hMyWeapons / 2
}

CBaseWeapon* CPlayer::GetWeap()
{
	DWORD hWeapon = *reinterpret_cast<DWORD*>(this + NetVars.m_hActiveWeapon);

	hWeapon &= 0xFFF;

	return Interfaces.EntList->GetClientEntity<CBaseWeapon>(hWeapon);
}

int CBaseWeapon::GetItemDefinitionIndex()
{
	return *reinterpret_cast<int*>(this + NetVars.m_iItemDefinitionIndex);
}